import sys

try:
    filename = sys.argv[1]
except:
    print('Give a file...')
    sys.exit(-1)
    
file = open(filename)
ll = file.readlines()[:-1]
file.close()

ll2 = ['0x' + ', 0x'.join(i.split()[1:]) + ',' for i in ll]

#print(ll2[0])

file = open('myfatdisk.h', 'w')

file.write('char myfatdisk[] = {\n')
for i in ll2[:-1]:
    file.write(i + '\n')
file.write(ll2[-1][:-1])
file.write('};\n')

file.close

