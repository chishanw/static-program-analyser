import os
import subprocess
from pathlib import Path

def main():
    autotester_path = "Team00/Code00/build_win/x86-Debug/src/autotester/autotester.exe" # TODO(Merlin): Please edit your path
    for folder_name in os.listdir("Team00/Tests00/"):

        source_dir = f"Team00/Tests00/{folder_name}/SourceFiles/"
        query_dir = f"Team00/Tests00/{folder_name}/QueryFiles/"
        out_xml_dir = f"Team00/Tests00/{folder_name}/XmlFiles/"

        # make out directory
        Path(out_xml_dir).mkdir(parents=True, exist_ok=True)

        source_filenames = os.listdir(source_dir)
        for source_fn in source_filenames:
            # Writing a new XML file
            source_fn_wo_extension = source_fn .split(".")[0]
            query_fn = source_fn_wo_extension + "Queries.txt"
            out_xml_fn = source_fn_wo_extension + "QueriesOut.xml"
            
            source_path = f'{source_dir}{source_fn}'
            query_path = f'{query_dir}{query_fn}'
            xml_path = f'{out_xml_dir}{out_xml_fn}'
            
            command = [autotester_path, source_path, query_path, xml_path]
            subprocess.run(command)
    input("Press enter to close program")

if __name__ == "__main__":
    main();