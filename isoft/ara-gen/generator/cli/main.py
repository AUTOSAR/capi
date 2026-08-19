#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------
"""
Entry point of aragen.
"""
import json
import logging
import multiprocessing
import os
import sys
from json import JSONDecodeError

from generator.common.aragen_args import AragenArgs
from generator.common.lxml_preparser import LxmlPreparser
from generator.common.method_util import get_all_arg, init_logging, GLOBAL_JUDGE
from generator.common.tree_helper import short_name
from generator.generator.generator import Generator
from generator.generator.generator_settings import GeneratorSettings
from generator.parser.default_parser import DefaultParser
from generator.parser.exception_handling import handle_exceptions


@handle_exceptions(error_string_template="ERROR: {exception_text}")
def main(argv=None):
    """
    Main entry point to aragen.
    """

    if argv is None:
        argv = sys.argv
    args = AragenArgs(argv)
    GLOBAL_JUDGE["debug"] = args.debug
    GLOBAL_JUDGE["check_uuid"] = args.check_uuid
    init_logging(args)
    log = logging.getLogger(__name__)
    log.info('process Starting up...')
    log.debug("Using input from %s", args.files)
    args = get_all_arg(args)
    if len(args.zeroList) > 0:
        log.warning("These files are all 0 bytes. [{}].".format(", ".join(args.zeroList)))
    judge = judge_project(args, log)
    if not judge:
        show_info = judge_show_info(args, log)
        if not show_info:
            if args.executables:
                get_build_exe(args, log)
            elif args.single_process:
                get_all_(args, log)
            else:
                jobs = []
                _generate_all = not (args.machines or args.processes or args.software_components or args.executables or args.softwarePackageFqn or args.software_cluster or args.vehiclePackageFqn)
                show_error_ob = judge_show_error(args, _generate_all)
                if _generate_all or args.softwarePackageFqn or args.software_cluster or args.vehiclePackageFqn or args.software_components or args.executables:
                    other = multiprocessing.Process(target=get_other, args=(args, show_error_ob["showOther"]))
                    jobs.append(other)
                if _generate_all or args.processes:
                    pt = multiprocessing.Process(target=get_process, args=(args, show_error_ob["showProcess"]))
                    jobs.append(pt)
                if _generate_all or args.machines:
                    mt = multiprocessing.Process(target=get_machine, args=(args, show_error_ob["showMachine"]))
                    jobs.append(mt)
                dt = multiprocessing.Process(target=get_diag, args=(args,))
                jobs.append(dt)
                for t in jobs:
                    t.daemon = True
                    t.start()
                for t in jobs:
                    t.join()
                    if t.exitcode != 0:
                        sys.exit(1)
                jsonDumps(args)
                log.info('Finished.')


def judge_show_error(args, showAll):
    show_error_ob = {"showOther": False,
                     "showMachine": False,
                     "showProcess": False}
    _have_other = args.softwarePackageFqn or args.software_cluster or args.vehiclePackageFqn or args.software_components or args.executables
    if showAll or _have_other:
        show_error_ob["showOther"] = True
    elif args.processes:
        show_error_ob["showProcess"] = True
    elif args.machines:
        show_error_ob["showMachine"] = True
    return show_error_ob


def judge_project(args, log):
    if 'PROJECT' in args.generate or 'PROJECT_UPDATE' in args.generate:
        used_parser = get_used_parser(args, True, True, False)
        args.single_process = True
        project = used_parser.get_project()
        gen = Generator(args)
        gen.generate(project, args.files)
        jsonDumps(args)
        log.info('Finished.')
        return True
    return False


def judge_show_info(args, log):
    if args.list_machines or args.list_swcl_info or args.list_processes or args.list_swp or args.list_swc:
        lxmlPreParser = LxmlPreparser()
        arxml_tree = lxmlPreParser.parse(args.files, "", True, args.no_reference_checking)
        if arxml_tree is not None:
            used_parser = get_used_schema(arxml_tree, args)
            used_parser.process_singletone_args(args, log)
        return True
    else:
        return False


@handle_exceptions(error_string_template="ERROR: {exception_text}")
def get_process(args, show_error):
    used_parser = get_used_parser(args, True, show_error)
    if used_parser is not None:
        process = used_parser.get_process_()
        gen = Generator(args)
        gen.generate(process, args.files)


@handle_exceptions(error_string_template="ERROR: {exception_text}")
def get_machine(args, show_error):
    used_parser = get_used_parser(args, True, show_error)
    if used_parser is not None:
        machine = used_parser.get_machine_()
        gen = Generator(args)
        gen.generate(machine, args.files)


@handle_exceptions(error_string_template="ERROR: {exception_text}")
def get_diag(args):
    used_parser = get_used_parser(args, False)
    if used_parser is not None:
        diag = used_parser.get_diag_()
        gen = Generator(args)
        gen.generate(diag, args.files)


@handle_exceptions(error_string_template="ERROR: {exception_text}")
def get_other(args, show_error):
    used_parser = get_used_parser(args, True, show_error)
    if used_parser is not None:
        other = used_parser.get_other_()
        gen = Generator(args)
        gen.generate(other, args.files)

@handle_exceptions(error_string_template="ERROR: {exception_text}")
def get_build_exe(args, log):
    used_parser = get_used_parser(args, True, True, False)
    if used_parser is not None:
        exe = used_parser.get_build_exe_()
        gen = Generator(args)
        gen.generate(exe, args.files)
    log.info('Finished.')


def get_all_(args, log):
    used_parser = get_used_parser(args, True, True, False)
    process = used_parser.get_process_()
    machine = used_parser.get_machine_()
    diag = used_parser.get_diag_()
    other = used_parser.get_other_()
    exe = used_parser.get_build_exe_()
    allOb = {**process, **machine, **diag, **other, **exe}
    gen = Generator(args)
    gen.generate(allOb, args.files)
    jsonDumps(args)
    log.info('Finished.')


def get_used_parser(args, need, showError=False, initLog=True):
    if initLog:
        init_logging(args)
        args = get_all_arg(args, True)
    lxmlPreParser = LxmlPreparser()
    arxml_tree = lxmlPreParser.parse(args.files, "", showError, args.no_reference_checking)
    if arxml_tree is not None:
        judge_some_fqn(arxml_tree, showError)
        used_parser = get_used_schema(arxml_tree, args)
        if need:
            stateStr = used_parser.create_state_some_ip()
            nmStr = used_parser.create_nm_xml()
            if len(stateStr+nmStr) > 0:
                arxml_tree = lxmlPreParser.parse(args.files, stateStr + nmStr, showError, args.no_reference_checking)
                used_parser = get_used_schema(arxml_tree, args)

        return used_parser


def judge_some_fqn(arxml_tree, showError):
    fqn_list = arxml_tree.findall_fqn()
    errorStr = ""
    for item in fqn_list:
        if len(fqn_list[item]) > 1:
            if errorStr == "":
                errorStr = "[INIT] CODE-005: "
            errorStr += "The following element \"{}\" is repeated in the same location {}. ".format(short_name(item), item)
    if errorStr != "" and showError:
        assert False, errorStr


def get_used_schema(arxml_tree, args):
    used_parser = DefaultParser(arxml_tree, args)
    return used_parser


def jsonDumps(args):
    generation_root = GeneratorSettings(args).output_dir
    for root, dirs, files in os.walk(generation_root):
        for dir in dirs:
            # ignore .xxx dir (e.g .vscode .idea)
            if dir[:1] == '.':
                continue
            dirPath = os.path.join(root, dir)
            files = os.listdir(dirPath)
            for file in files:
                if file.endswith(".json"):
                    filePath = os.path.join(root, dir, file)
                    with open(filePath) as load_f:
                        data = load_f.read()
                        load_f.close()
                        if len(data) < 1:
                            continue
                        dictJson = remove_redundant_comma(data)
                        data = json.dumps(dictJson, indent=4, ensure_ascii=False)
                        with open(filePath, "w") as fp:
                            fp.write(data)
                            fp.close()


def remove_redundant_comma(data):
    try:
        dictJson = json.loads(data)
    except JSONDecodeError:
        result = " ".join(line.strip() for line in data.splitlines())
        data = result.replace(" ", "").replace(",}", "}").replace(",]", "]").replace("[,", "[").replace("{,", "{")
        dictJson = json.loads(data)
    return dictJson

