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
import logging
import os

from lxml import etree
GLOBAL_LOCAL_PATH = {}
GLOBAL_JUDGE = {"debug": False,
                "check_uuid": False
                }

def get_all_arg(args, isHave=False):
    xmls = args.files
    args.files = []
    for x in xmls:
        # find in dir
        if x[-6:].upper() != '.ARXML':
            assert os.path.exists(x), "path not exists: " + x
            for xml in find_all_arxmls(x):
                zero_file(args, xml, isHave)
        else:
            zero_file(args, x, isHave)
    args.files = sorted(list(set(args.files)), key=str.lower)
    return args

def zero_file(args, arxml, isHave):
    realpath = os.path.realpath(os.path.abspath(arxml))
    if os.path.getsize(realpath) == 0:
        if not isHave:
            args.zeroList.append(realpath)
    else:
        args.files.append(realpath)

def find_all_arxmls(base):
    for root, ds, fs in os.walk(base):
        for f in fs:
            if f.endswith('.arxml'):
                fullname = os.path.join(root, f)
                yield fullname


def init_logging(args):
    root_level = logging.WARNING - 10 * args.verbose
    if args.verbose >= 3:
        root_level = logging.TRACE
    if GLOBAL_JUDGE["debug"]:
        log_format = '{asctime} - [{levelname:<8}] [ARAGEN  ] {name} {message}'
    else:
        log_format = '{asctime} - [{levelname:<8}] [ARAGEN  ] {message}'
    log_handlers = [logging.NullHandler()]
    if not args.quiet:
        log_handlers.append(logging.StreamHandler())
    if args.log:
        log_handlers.append(logging.FileHandler(args.log))
    logging.basicConfig(level=root_level, format=log_format, style='{', handlers=log_handlers)


def remove_nil(filePath):
    tree = etree.parse(open(filePath, encoding='utf-8'))
    div_nil = tree.xpath("//*[@*[name()='xsi:nil'][.=1 or .='true']]")
    for item in div_nil:
        item.getparent().remove(item)
    empty_tags = tree.xpath("//*[not(node())]")
    for tag in empty_tags:
        if "GID" in tag.attrib and tag.attrib["GID"] == "iSOFT:diag:DiagnosticAccessPermission":
            pass
        else:
            tag.getparent().remove(tag)
    # if div_nil:
    #     string = etree.tostring(tree).decode('utf-8')
    #     print(string)
    return tree

def check_uuid(tree, uuidList, repeatUuidList):
    if GLOBAL_JUDGE["check_uuid"]:
        xpath_str = ".//*/@{}".format("UUID")
        uuids = tree.xpath(xpath_str)
        for uuid in uuids:
            if uuid != "":
                if uuid in uuidList:
                    if uuid not in repeatUuidList:
                        repeatUuidList.append(uuid)
                else:
                    uuidList.append(uuid)