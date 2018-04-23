import sys
import os
import shutil

if len(sys.argv) < 2:
    print("Usage: makedist source dest")
    sys.exit(1)

source = sys.argv[1]
dest = sys.argv[2]

print("Creating Qt WebAassembly examples distribution.")
print("Source:      {0}".format(source))
print("Destination: {0}".format(dest))

# process each source dir
dirs = sorted(list([f for f in os.listdir(source) if os.path.isdir(os.path.join(source, f))]))
for dir in dirs:
    sourcepath = os.path.join(source, dir)
    destpath = os.path.join(dest, dir)
    
    # find all files
    files = [os.path.join(dp, f) for dp, dn, fn in os.walk(sourcepath) for f in fn]
    
    # skip some intermediate build artifacts
    def skipFile(path):
        return path.endswith(".cpp") or path.endswith(".h") or path.endswith(".o") or path.endswith("Makefile") \
               or path.endswith(".moc") or path.endswith(".stash") or path.endswith(".cache") or path.endswith(".qrc")
    prunedFiles = list(filter(lambda x: not skipFile(x), files))

    # copy all files
    print("")
    print("mkdir {0}".format(destpath))
    os.makedirs(destpath, exist_ok=True)
    for file in prunedFiles:
        filePath = os.path.join(sourcepath, file)
        print("  copy {0}".format(file))
        shutil.copy(filePath, destpath)

# create index.html
html = "<html>\n<head><title>Qt For WebAssembly Examples</title></head>\n<body>\n"
html += "Qt For WebAssembly Examples<br><br>\n"
for dir in dirs:
    html += "<a href={0}>{1}</a> <br>\n".format(dir + "/" + dir + ".html", dir)
html += "</body>\n</html>\n"

htmlfile = os.path.join(dest, "index.html")
print("")
print("Create {0}".format(htmlfile))
f = open(htmlfile, 'w')
f.write(html)
print("")

