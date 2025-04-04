from collections import Counter
import re

file_name = input ("Enter filename:")

with open("pairs.dot", "r") as f:
    lines = f.readlines()

edge_pattern = re.compile(r'^\s*"(.+?)"\s*->\s*"(.+?)"')

edges = []
other_lines = []

for line in lines:
    match = edge_pattern.match(line)
    if match:
        edges.append((match.group(1), match.group(2)))
    else:
        other_lines.append(line)

edge_counts = Counter(edges)

with open(file_name, "w") as f:
    f.writelines([line for line in other_lines if "->" not in line])  

    for (src, dst), count in edge_counts.items():
        label = f' [label="x{count}"]' if count > 1 else ""
        f.write(f'  "{src}" -> "{dst}"{label};\n')

    f.write("}\n")  


