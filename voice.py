#!/usr/bin/env python

import argparse
import asyncio
import glob
import pathlib
import random
import re
import shelve
from typing import Union, List


import aioserial


# 1. Start - wait for messages asking for initial state
# 2. Respond with the current state
# 3. Wait for "eyes to close" and recod every motor update
# 4. enter operational state
# 5. Wait for sound messages
#   5a. Play sound
# 6. if asked for current state - go back to 1

EYE_POSITION = "eye_position"
GET_EYE_POSITION = "get_eye_position"
STORAGE_NAME = "storage/picture_store"
STATE_PATTERN = re.compile(r"eyes: (\d+) closed: (yes|no) sound: (hello|goodbye|random|)")
BAUD_RATE = 115200


# Types
FilePath = Union[pathlib.Path, str]


def get_sound_file_paths(sound_dir: pathlib.Path) -> List[FilePath]:
    """
    Iterate over all of the files in the sound directory matching the '*.raw'
    extension, verify they are valid files and return a list of all paths

    @param sound_dir - directory containing raw sound files
    @returns list of paths to these files
    """
    sound_files: List[FilePath] = []
    for ext in ("*.raw", "*.RAW", "*.mp3", "*.wav"):
        sound_files.extend(glob.glob(f"{sound_dir}/{ext}"))

    return sound_files


def record_eye_position(position: int, shelf: shelve.DbfilenameShelf):
    """
    Take in the position of the motor and record it in the file backed shelf

    @param position - integer position of the motor
    @param shelf - file backed storage
    """
    if position is not None:
        shelf[EYE_POSITION] = position


def get_eye_position(shelf: shelve.DbfilenameShelf):
    return shelf.get(EYE_POSITION, 0)


async def initial_state(serial: aioserial.AioSerial, shelf: shelve.DbfilenameShelf):
    print(f"Entering the init state with motor in position: {get_eye_position(shelf)}")
    while True:
        msg = (await serial.readline_async(-1)).decode(errors="ignore")
        print(msg)
        if msg.startswith(GET_EYE_POSITION):
            eye_motor_position = get_eye_position(shelf)
            written_bytes = await serial.write_async(f"{eye_motor_position}".encode("utf-8"))
            break
    print("Arduino motor initialized")


def unpack_msg(state_msg: str):
    state_match = STATE_PATTERN.match(state_msg.rstrip('\n'))
    if state_match:
        eye_pos, closed, sound = state_match.groups()
        eye_pos = int(eye_pos)
        return eye_pos, closed, sound

    return (None,) * 3


async def eyes_in_position(serial: aioserial.AioSerial, shelf: shelve.DbfilenameShelf):
    print("Wating for the eyes to get to the closed position")
    while True:
        msg = (await serial.readline_async(-1)).decode(errors="ignore")
        eye_pos, closed, *_ = unpack_msg(msg)
        print(f"eye pos: {eye_pos}, closed: {closed}, sound: {_}")
        record_eye_position(eye_pos, shelf)
        if closed is not None and closed == "yes":
            print("Eyes arrived in the closed position")
            break
    print("Eyes are closed")


class SoundPlayer:
    """
    Plays given sounds
    """
    def __init__(self):
        self._playing = False

    @property
    def is_playing(self):
        return self._playing

    async def _start_playing(self, sound_file) -> None:
        try:
            self._playing = True
            proc = await asyncio.create_subprocess_exec("aplay", sound_file)
            code = await proc.wait()
            if code != 0:
                print("Sound player encountered an error")
        except Exception as error:
            print(f"Error playing sound: {error}")
        finally:
            self._playing = False

    def start_playing(self, sound_file):
        asyncio.get_event_loop().call_soon(asyncio.create_task, self._start_playing(sound_file))


class SoundsDB:
    """
    Contains all of the sound files that the picture knows how to play
    """
    def __init__(self, hello_dir, bye_dir, random_dir):
        """
        Initialize a new instance of SoundsDB

        @param hello_dir - directory containing hello sounds
        @param bye_dir - directory containing goodbye sounds
        @param random_dir - directory containing random sounds
        """
        self._hellos = get_sound_file_paths(hello_dir)
        self._goodbyes = get_sound_file_paths(bye_dir)
        self._randoms = get_sound_file_paths(random_dir)

    def get_sound_file(self, sound_type: str) -> FilePath:
        """
        Lookup the sound file by the given type

        @param sound_type - category of sound the user is interested in: hello, goodbye or random
        @returns path to the sound file
        """
        if sound_type == "hello":
            return random.choice(self._hellos)
        elif sound_type == "goodbye":
            return random.choice(self._goodbyes)
        else:
            return random.choice(self._randoms)


def create_parser():
    """
    Helper function to create the command line arguments parser

    @returns instance of ArgumentParser
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("-d",
                        "--device",
                        help="Device file to communicate with arduino",
                        type=argparse.FileType('r'),
                        required=True)
    parser.add_argument("-g",
                        "--greeting_dir",
                        help="Directory containg greeting sound files",
                        type=str,
                        default="hellos")
    parser.add_argument("-b",
                        "--bye_dir",
                        help="Directory containg goodbye sound files",
                        default="goodbyes",
                        type=str)
    parser.add_argument("-r",
                        "--random_dir",
                        help="Directory containg random sound files",
                        default="randoms",
                        type=str)

    return parser


async def main():
    parser = create_parser()
    args = parser.parse_args()

    serial = aioserial.AioSerial(port=args.device.name, baudrate=BAUD_RATE)
    initialized = False
    running = True
    player = SoundPlayer()
    sounds_db = SoundsDB(args.greeting_dir, args.bye_dir, args.random_dir)

    with shelve.open(STORAGE_NAME) as shelf:
        while running:
            if not initialized:
                await initial_state(serial, shelf)
                await eyes_in_position(serial, shelf)
                print("Picture is initialized")
                initialized = True

            msg = (await serial.readline_async(-1)).decode(errors="ignore")
            eye_pos, closed, sound = unpack_msg(msg)
            record_eye_position(eye_pos, shelf)

            if msg == GET_EYE_POSITION:
                initialized = False
                continue

            print(f"sound {sound} player: {player.is_playing}")
            valid_sound = sound is not None and len(sound.strip()) > 0
            if valid_sound and not player.is_playing:
                sound_path = sounds_db.get_sound_file(sound)
                player.start_playing(sound_path)

try:
    asyncio.run(main())
except (KeyboardInterrupt, SystemExit):
    print("Exited due to interrupt...")
