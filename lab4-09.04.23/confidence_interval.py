import numpy as np
import scipy.stats as st
import sys
from numbers import Number
from dataclasses import dataclass


def confident_interval(data: list[Number]):
    return st.t.interval(0.95, df=len(data)-1, loc=np.mean(data), scale=st.sem(data))    


@dataclass
class Row:
    N: int
    min_MP: int
    ci_MP: tuple[float]
    min_NOMP: int
    ci_NOMP: tuple[float]

    def __str__(self) -> str: 
        deserialize = lambda it: tuple(map(lambda t: format(t,".2f"), it))
        return f"{self.N},{self.min_MP},{deserialize(self.ci_MP)},{self.min_NOMP},{deserialize(self.ci_NOMP)}"


def analyze(data_line: str) -> str:
    deserialize = lambda s: [int(it) for it in s.replace('[', '').replace(']', '').split(';')]
    N, MP, NOMP = data_line.split(',')
    arr_MP, arr_NOMP = deserialize(MP), deserialize(NOMP)
    return str(
        Row(
                N, 
                min(arr_MP), 
                confident_interval(arr_MP), 
                min(arr_NOMP), 
                confident_interval(arr_NOMP)
            )
        )


data_filename = sys.argv[1]
with open(data_filename, 'r') as f:
    raw_content = f.readlines()
del raw_content[0]
analyzed_data = list(map(analyze, raw_content))
with open(f'analyzed_{data_filename}', 'w') as f:
    f.write('N,min_MP,ci_MP,min_NOMP,ci_NOMP\n')
    for l in analyzed_data:
        f.write(f'{l}\n')
