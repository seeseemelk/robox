from enum import Enum
from pydantic import BaseModel, validator
from pathlib import Path
import wave
import typer

from typing import List

PREAMBLE = '0000000011111111'

###############################################################################
# classes representing 1 ihex line ############################################

class RecordType(Enum):
    data = 0
    end_of_file = 1
    extended_segment_adress = 2
    start_segment_address = 3
    extended_linear_address = 4
    start_linear_address = 5


class HexLine(BaseModel):
    byte_count: int
    address: int
    record_type: RecordType
    data: List[int]
    checksum: int

    class Config:
        frozen = True

    @validator('data', pre=True)
    def data_validator(cls, v, values, **kwargs):
        if isinstance(v, str):
            lst = [int(v[x:x+2], 16) for x in range(0, values['byte_count']*2, 2)]
            return lst

    def recreate(self):
        data = ''.join([f'{i:02x}' for i in self.data])
        return f':{self.byte_count:02x}{self.address:04x}{self.record_type.value:02x}{data}{self.checksum:02x}'

###############################################################################
# representing an entire hex file #############################################

class IHex:
    def __init__(self) -> None:
        self.ihex: List[HexLine] = []

    def read_hex(self, file: str) -> List[HexLine]:
        res: List[HexLine] = []

        with open(file) as f:
            raw_hex = f.read()

        for l in raw_hex.split('\n'):
            if l.startswith(':'):
                res.append(HexLine(
                    byte_count = int(l[1:3], 16),
                    address = int(l[3:7], 16),
                    record_type = RecordType(int(l[7:9], 16)),
                    data = l[9:-2],
                    checksum = int(l[-2:], 16)
                ))

        self.ihex = res

        return res

    def recreate(self) -> str:
        res = ''
        for l in self.ihex:
            res += l.recreate() + '\n'

        return res

    def bitstream(
            self, 
            include_address: bool = True,
            preamble_after_bits: int = 0,
            preamble_length: int = 1,
        ) -> str:

        data_bits = ''.join([PREAMBLE]*preamble_length)

        for e, l in enumerate(self.ihex):
            if (l.record_type == RecordType.data):
                if include_address:
                    data_bits += f'{l.address:016b}'

                data_bits += ''.join([f'{x:08b}' for x in l.data])

            if (preamble_after_bits > 0) and ((e % preamble_after_bits) == 0):
                data_bits += ''.join([PREAMBLE]*preamble_length)

        return data_bits

    def create_wave(
            self,
            file: Path,
            symbols_second: int = 1000,
            preamble_after_bits: int = 0,
            preamble_length: int = 1,
        ) -> None:
        MIN = 0
        MAX = 255

        nchannels = 2
        sampwidth = 1
        framerate = 44100
        nframes = 0
        compname = 'not compressed'
        comptype = 'NONE'

        frame_reapeat = framerate / symbols_second
        quater_frame = int(frame_reapeat / 4)

        f = open(file, 'wb')
        w = wave.Wave_write(f)
        w.setparams((nchannels, sampwidth, framerate, nframes, comptype, compname))

        clk = [*[MIN]*quater_frame, *[MAX]*quater_frame*2, *[MIN]*quater_frame]
        

        for bit in self.bitstream(preamble_after_bits, preamble_length):
            if bit == '0':
                data = [*[MIN]*quater_frame*2, *[MAX]*quater_frame*2]

            else:
                data = [*[MAX]*quater_frame*2, *[MIN]*quater_frame*2]

            for x, y in zip(data, clk):
                w.writeframes( bytes([x, y]) )

        w.close()

###############################################################################
# command line function wrappers ##############################################

def generate_audio(
        input_: Path = typer.Argument(..., help='ihex file to convert to audio'),
        output: Path = typer.Option('output.wav', help='output audio filename'),
        symbols_second: int = typer.Option(2000, help='rate at which the symbols will be send'),
        preamble_after_bits: int = typer.Option(0, help='add a preamble sequence after x bits (will always be added at the start)'),
        preamble_length: int = typer.Option(1, help='make the preamble sequence longer by repeating it'),
    ) -> None:

    i = IHex()
    i.read_hex(input_)
    i.create_wave(
        output,
        symbols_second,
        preamble_after_bits,
        preamble_length,
    )


if __name__ == '__main__':
    typer.run(generate_audio)
    # i = IHex()
    # i.read_hex('bin/robox.txt')
    # # i.read_hex('/home/www/owl_20210224/targets/owl_tn461_20220316203613.hex')
    # # print(i.recreate())

    # print(i.bitstream())
    # i.create_wave('test.wav')
    