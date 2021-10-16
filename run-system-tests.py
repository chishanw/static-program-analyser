import os
import sys
import math
import argparse
import subprocess
from pathlib import Path
import xml.etree.ElementTree as ET
import xml.dom.minidom


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-w", "--win_mode", help="switch to win mode",
                        action="store_true")
    parser.add_argument("-v", "--verbose", help="print passed queries too",
                        action="store_true")
    args = parser.parse_args()

    ci_failed = False
    number_queries = 0
    total_time_taken = 0
    max_time_taken = -math.inf
    min_time_taken = math.inf
    avg_time_taken = 0

    autotester_path_mac = "Team02/Code02/build/src/autotester/autotester"
    autotester_path_win = "Team02/Code02/build/src/autotester/Release/autotester"

    if args.win_mode:
        autotester_path = autotester_path_win
    else:
        autotester_path = autotester_path_mac

    test_folder_dir = "Team02/Tests02/"
    out_xml_dir = "Team02/Tests02/XmlFiles/"

    # make out directory
    Path(out_xml_dir).mkdir(parents=True, exist_ok=True)

    source_fns = []
    for fn in os.listdir(test_folder_dir):
        if fn.endswith("_source.txt"):
            source_fns.append(fn)

    for source_fn in source_fns:

        # <TEST-DESCRIPTION>_source.txt
        # <TEST-DESCRIPTION>_queries.txt
        source_fn_wo_suffix = source_fn[:-len("_source.txt")]
        query_fn = source_fn_wo_suffix + "_queries.txt"
        out_xml_fn = source_fn_wo_suffix + "_out.xml"

        source_path = f'{test_folder_dir}{source_fn}'
        query_path = f'{test_folder_dir}{query_fn}'
        xml_path = f'{out_xml_dir}{out_xml_fn}'

        command = [autotester_path,
                   source_path, query_path, xml_path]

        print(f"[INFO] Query file: {query_path}")

        output = subprocess.run(command, capture_output=True)

        # if autotester failed
        if output.returncode:
            print(f"[ERROR][Autotester] Program quit unexpectedly")
            print(f"return code: {output.returncode}")
            print(f"autotester output:")
            print(output.stdout.decode())
            ci_failed = True
            continue

        # parse xml file only when autotester runs without error
        try:
            with open(f'{out_xml_dir}{out_xml_fn}', 'r') as xml_file:
                xml_file_data = xml_file.read().replace('\n', '')
                this_failed, this_number_queries, this_total_time_taken, this_max_time_taken, this_min_time_taken = parse_xml_and_print_stat(
                    xml_file_data, verbose=args.verbose)
        except Exception as e:
            print("[ERROR][ParseXML] Failed to parse xml file")
            print(e)

        if this_failed:
            ci_failed = True

        number_queries += this_number_queries
        total_time_taken += this_total_time_taken
        max_time_taken = max(max_time_taken, this_max_time_taken)
        min_time_taken = min(min_time_taken, this_min_time_taken)

    if number_queries:
        avg_time_taken = total_time_taken / number_queries

    print()
    print("[INFO] Performance Stats")
    print(f"number_queries: {number_queries}")
    print(f"total_time_taken (ms): {total_time_taken:.3f}")
    print(f"max_time_taken (ms): {max_time_taken:.3f}")
    print(f"min_time_taken (ms): {min_time_taken:.3f}")
    print(f"avg_time_taken (ms): {avg_time_taken:.3f}")

    if ci_failed:
        exit(1)


def parse_xml_and_print_stat(xml_str, verbose):
    number_queries = 0
    total_time_taken = 0
    max_time_taken = -math.inf
    min_time_taken = math.inf
    failed = False

    tree = ET.fromstring(xml_str)

    for query in tree.iter("query"):

        query_comment = query.find("id").attrib["comment"]
        time_taken = float(query.find("time_taken").text)

        if query.find("failed"):
            failed = True

            print(f"[ERROR][Query] Failed: {query_comment}")
            failed_xml_str = ET.tostring(query, encoding='utf-8', method="xml",
                                         short_empty_elements=False).decode("utf-8")
            dom = xml.dom.minidom.parseString(failed_xml_str)
            print(dom.toprettyxml())

        else:
            if verbose:
                print(f"[INFO][Query] Passed: {query_comment}")
                print(
                    f"<time_taken>{query.find('time_taken').text}</time_taken>")

        number_queries += 1
        total_time_taken += time_taken
        max_time_taken = max(max_time_taken, time_taken)
        min_time_taken = min(min_time_taken, time_taken)

    return failed, number_queries, total_time_taken, max_time_taken, min_time_taken


if __name__ == "__main__":
    main()
