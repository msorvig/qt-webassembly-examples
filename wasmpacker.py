import os
import argparse
import sys
import itertools
import pathlib
import gzip

parser = argparse.ArgumentParser(description="Compresses Qt WebAssembly applications for distribution")
parser.add_argument("src", help="Where to get the files", nargs=1, type=str)
parser.add_argument("dst", help="Where to put the files", nargs="?", type=str)
parser.add_argument("--compress-gzip", help="Compress using gzip", action='store_true')
parser.add_argument("--compress-brotli", help= "Compress using brotli", action='store_true')
args = parser.parse_args()

def allFiles(path):
    for dirpath, _, filenames in os.walk(path):
        for filename in filenames:
            yield os.path.join(dirpath, filename)

def shipItFiles(path):
    # skip some intermediate build artifacts
    def skipFile(path):
        return path.endswith(".cpp") or path.endswith(".h") or path.endswith(".o") or path.endswith("Makefile") \
               or path.endswith(".moc") or path.endswith(".stash") or path.endswith(".cache") or path.endswith(".qrc")
    return itertools.filterfalse(skipFile, allFiles(path))

def compressedFiles(path):
    for filepath in shipItFiles(path):
        try:
            filecontent = pathlib.Path(filepath).read_bytes()
        except FileNotFoundError:
            print("Skip symlink or unreadable {0}".format(filepath))
            continue

        if (args.compress_gzip):
           compressedContent = gzip.compress(filecontent)
           yield filepath, compressedContent
           continue

        if (args.compress_brotli):
           try:
               import brotli
           except ImportError:
               print("Error: Brotli compressor is not installed, run pip(3) install brotli")
               exit(1)
           compressedContent = brotli.compress(filecontent)
           yield filepath, compressedContent
           continue

        yield filepath, filecontent

def writeFile(relativeFilePath, fileContent, destPath):
    fullDestPath = os.path.join(destPath, relativeFilePath)
    if (args.compress_gzip):
        fullDestPath += ".gz"
    if (args.compress_brotli):
        fullDestPath += ".br"
    if os.path.isfile(fullDestPath):
        print("Destination file exists, skip {0}".format(fullDestPath))
        return

    print("Write {0}".format(fullDestPath))
    destDir = os.path.dirname(fullDestPath)
    os.makedirs(destDir, exist_ok=True)
    pathlib.Path(fullDestPath).write_bytes(fileContent)

srcDir = str(args.src[0])
srcPath = os.path.abspath(srcDir)
dstPath = os.path.abspath(str(args.dst)) if args.dst else None

print("Process files from {0}, store in {1}".format(srcPath, dstPath))

for filePath, content in compressedFiles(srcPath):
    relativeFilePath = os.path.relpath(filePath, srcPath)
    if dstPath is not None:
        _, tailPath = os.path.split(srcDir)
        writeFile(relativeFilePath, content, os.path.join(dstPath, tailPath))
    else:
        print ("File {0} Size {1}".format(filePath, len(content)))


