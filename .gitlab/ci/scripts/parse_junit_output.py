import sys
import xml.etree.ElementTree as ET
from pathlib import Path

xml_path = ""
if len(sys.argv) > 1:
    xml_path = sys.argv[1]
else:
    raise ValueError("please supply the location of the junit xml file")

base_path = Path().resolve()
print(base_path)

tree = ET.parse(xml_path)
root = tree.getroot()

for testcase in root.iter("testcase"):
    filename = testcase.attrib.get("file") or testcase.attrib.get("filename")
    if filename:
        abs_path = Path(filename).resolve()
        try:
            rel_path = abs_path.relative_to(base_path)
            testcase.set("file", str(rel_path))
        except ValueError:
            pass

tree.write(xml_path, encoding="utf-8", xml_declaration=True)
