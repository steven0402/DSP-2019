#! /usr/bin/python
import sys

in_file = sys.argv[1]
out_file = sys.argv[2]

word_dict = {}
with open(in_file, "r", encoding="big5hkscs") as fin:
    for line in fin:
        temp = line.split(' ')
        juyins_set = temp[1].split('/')
        for juyin in juyins_set:
            first_juyin = juyin[0]
            if first_juyin in word_dict:
                if temp[0] not in word_dict[first_juyin]:
                    word_dict[first_juyin].append(temp[0])
            else:
                word_dict[first_juyin] = [temp[0]]
        word_dict[temp[0]] = temp[0]

with open(out_file, "w", encoding="big5hkscs") as fout:
    for key in sorted(word_dict):
        fout.write(key)
        count = 0
        for item in word_dict[key]:
            if count == 0:
                fout.write('\t')
            else:
                fout.write(' ')
            fout.write(item)
            count += 1
        fout.write('\n')

    


