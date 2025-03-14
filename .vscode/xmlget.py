import xml.etree.ElementTree as ET
import json
import re
import os
import sys
import shutil

def get_cmsis_path(xml_root):
    # 获取 Keil MDK 的安装路径
    uv4_path = shutil.which('UV4')
    if not uv4_path:
        return None
    
    keil_root = os.path.dirname(os.path.dirname(uv4_path))
    packs_path = os.path.join(keil_root, 'ARM', 'Packs')
    
    # 查找 CMSIS 包的版本号
    cmsis_package = xml_root.find(".//component[@Cclass='CMSIS']/package")
    if cmsis_package is None:
        return None
    version = cmsis_package.get('version')

    # 构建完整的 package 路径
    package_path = os.path.join(packs_path, 'ARM', 'CMSIS',
                               version, "CMSIS", "Core", "Include")
    return package_path

def check_armcc_version(compiler_text):
    """检查是否为 ARMCLANG 编译器并返回版本号"""
    if compiler_text and 'ARMCLANG' in compiler_text:
        version = compiler_text.split('::')[0]
        if version and int(version) >= 6100100:
            return version
    return None

settings_json_path = '.vscode\\settings.json'
mdk_prj_path = 'MDK-ARM\\project.uvprojx'

# 检查是否存在.uvprojx文件
if (len(mdk_prj_path) == 0):
    print("No project found.")
    sys.exit(0)

# 解析XML文件
tree = ET.ElementTree(file=mdk_prj_path)
root = tree.getroot()
IncludePath = root.find('.//TargetArmAds/Cads//IncludePath').text
Define = root.find('.//TargetArmAds/Cads//Define').text

# 获取 Defines 列表
if (Define != None):
    define_list = re.findall('[^, ]+', Define) # 这样提取出来后是一个list
else:
    define_list = []

compiler = root.find('.//pCCUsed').text
version = check_armcc_version(compiler)
if version:
    define_list.append(f'__ARMCC_VERSION={version}')

# 获取 Include Path
if(IncludePath != None):
    raw_list = re.findall(r'(?:\.{2}|\\)[\\/]([^;]+)(?:;|$)', IncludePath) # 这样提取出来后是一个list
else:
    raw_list = []

compare_flag = False
path_list = []

# 排除重复的路径
for path in raw_list:
    for path_temp in path_list:
        if path == path_temp:
            compare_flag = True
    if compare_flag == True:
        compare_flag = False
    else:
        path_list.append(path)

if(get_cmsis_path(root)!= None):
    path_list.append(get_cmsis_path(root))

# 检查是否存在settings.json文件
if os.path.isfile(settings_json_path) != True:
    print("'settings.json' does not exist.")
    sys.exit(0)

with open(settings_json_path, "r", encoding='utf-8') as json_file:
    json_raw = json.load(json_file) # 将Json文件读取为Python对象

# 删除原来的 includePath 和 defines
if('C_Cpp.default.includePath' in json_raw):
    del json_raw['C_Cpp.default.includePath']
if('C_Cpp.default.defines') in json_raw:
    del json_raw['C_Cpp.default.defines']

# 重新添加 includePath 和 defines
json_raw['C_Cpp.default.includePath'] = path_list
json_raw['C_Cpp.default.defines'] = define_list

# 将修改保存到文件
with open(settings_json_path, "w", encoding='utf-8') as json_file:
    json.dump(json_raw, json_file, indent=4)

print("Done.")
