import sys

assert len(sys.argv) == 3
file1 = sys.argv[1]
file2 = sys.argv[2]

with open(file1) as f:
    contentFile1 = f.readlines()
contentFile1 = [x.strip() for x in contentFile1]

with open(file2) as f:
    contentFile2 = f.readlines()
contentFile2 = [x.strip() for x in contentFile2]

assert(len(contentFile1) == len(contentFile2))

for i in range(len(contentFile1)):
	assert contentFile1[i] == contentFile2[i]

print("files are equal")