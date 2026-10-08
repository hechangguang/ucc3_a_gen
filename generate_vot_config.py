#!/usr/bin/env python3 
# -*- coding: utf-8 -*-
# 功能：独立实现CRC16-Modbus计算与16字节填充（无外部依赖）
# 日期：2025-09-09 10:07（农历乙巳蛇年七月十八）
 
import argparse 
import sys
import os 
import re 
from datetime import datetime 
from typing import Optional 
import shutil 
from pathlib import Path 
from ctypes import *
import struct 
 
 


class UserCfgNodeS(Structure):
    _fields_ = [
        ("cfg_name", c_char * 16),  # 固定16字节字符数组
        ("offset", c_uint32),
        ("size", c_uint32)
    ]
 
class UserCfgS(Structure):
    _fields_ = [
        ("cfg_base_addr", c_uint32),
        ("cfg_total_bytes", c_uint32),
        ("cfg_node_nums", c_uint32),
        ("cfg_reserved_bytes", c_uint32),
        ("cfg_node_array", UserCfgNodeS * 64)  # 固定64元素数组
    ]



def pad_file_to_size(filename, target_size) :
    """
    将文件填充到指定大小，返回填充数据的字节数组 
    参数:
        filename (str): 文件名
        target_size (int): 目标大小(字节)
        
    返回:
        list: 填充数据的字节数组
    """
    # 获取原始文件大小
    original_size = os.path.getsize(filename) 
    print(f"原始大小: {original_size}B 目标大小: {target_size}B | filename: {filename}")

    # 检查是否需要填充
    if original_size >= target_size:
        return []
    
    # 计算需要填充的大小 
    pad_size = target_size - original_size
    
    # 生成填充数据(这里用0x00填充)
    padding_data = bytes([0xFF] * pad_size)

    # 读取固件数据
    with open(filename,  'rb') as f:
        input_data = f.read() 
    
    whole_bytes = bytes(input_data + padding_data)
    # 方法1：Python 3.8+专用（推荐）
    #try:
        #print(f"填充数据（{pad_size}字节）: {whole_bytes.hex('  ').upper()}")  # 带空格分隔
    #except TypeError:
    ## 方法2：全版本兼容 
        #hex_str = ' '.join(f"{b:02X}" for b in whole_bytes)
        #print(f"adaptive 填充数据（{pad_size}字节）: {hex_str}")

    # 返回填充数据的字节列表 
    return whole_bytes


def struct_to_bytes(cfg):
    """将结构体实例转为字节流"""
    return bytes(cfg)
 
def bytes_to_struct(data):
    """字节流还原为结构体"""
    cfg = UserCfgS()
    memmove(addressof(cfg), data, sizeof(cfg))
    return cfg

# 初始化函数 
def init_with_ff(cfg):
    memset(addressof(cfg), 0xFF, sizeof(cfg))

def generate_user_cfg() -> bytes: 
    cfg = UserCfgS()
    init_with_ff(cfg)
    cfg.cfg_base_addr  = 0x100000
    cfg.cfg_node_nums  = 0 
    cfg.cfg_total_bytes = 0 
    cfg.cfg_reserved_bytes = 0
    
    current_path = os.path.abspath(__file__) 
    #base_path = Path(current_path).parent 
    base_path = Path(current_path).parent
    #print(f"当前脚本路径：{current_path},  {base_path}") 

    # 初始化节点数组 
    usrcfg_data=bytes(0)  
    cfg.cfg_node_nums = 0

    file_name = base_path / "isp_cfg" /  "isp_indoor.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp2_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp2_data
                cfg.cfg_total_bytes += len(isp2_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"isp_indoor", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 


    file_name = base_path / "isp_cfg" / "isp_k2_dde_weak_drc22.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"isp_outdoor", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_outdoor_k2.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"low_atd", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_outdoor_fly.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"pod_view", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_outdoor_fly_enh.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"pod_view2", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_manual.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"manual", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 
    
    file_name = base_path / "isp_cfg" / "isp_rtd6_02.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"rtd6_02", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 
 
    file_name = base_path / "isp_cfg" / "guan_niao_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"guan_niaov1", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_atd_dde_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev1", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_atd_dde_v2.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev2", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_atd_dde_v3.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev3", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_adt_v4.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev4", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_adt_sence_v5.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev5", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "low_adt_sence_nr_v6.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"lowatd_ddev6", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 


    file_name = base_path / "isp_cfg" / "high_light_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"high_light", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "white_hot_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"white_hot", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "black_hot_v0.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"black_hot", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "white_hot_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"iron_hot", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "white_hot_v1.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"red_hot", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_bpc.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"isp_bpc", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_iso_mode.bin"  #等温iso配置 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"isp_iso", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    file_name = base_path / "isp_cfg" / "isp_384_low_atd_v01.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"384_lowatd", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 
    
    
    file_name = base_path / "isp_cfg" / "low_adt_384_reverse_black-v2.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"384_lowatdrb", 4096*cfg.cfg_node_nums, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 

    cfg.cfg_total_bytes += len(bytes(cfg))
    print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}")   
    usr_cfg_header = bytes(cfg)  

    print(f"cfg.cfg_node_nums: {cfg.cfg_node_nums}")  

    # 序列化为字节流 
    byte_data = usr_cfg_header + usrcfg_data 


    print(f"序列化长度: {len(byte_data)}字节")  # 输出: 2068字节 

    # 反序列化验证
    #restored_cfg = bytes_to_struct(byte_data)
    #print(restored_cfg.cfg_node_array[0].cfg_name)   # 输出: b'config1' 
    #print(restored_cfg.cfg_node_array[1].cfg_name)   # 输出: b'config1' 

    return byte_data 


def rename_file_with_overwrite(old_path: str, new_filename: str) -> bool:
    """
    支持覆盖的带路径文件重命名 
    
    参数:
        old_path (str): 带路径的原文件（如：/data/old.txt ）
        new_filename (str): 不带路径的新文件名（如：new.txt ）
        
    返回:
        bool: 操作是否成功 
        
    特性:
        1. 自动处理路径拼接 
        2. 支持强制覆盖已存在文件 
        3. 跨平台路径处理 
        4. 原子化操作保障（避免操作中断导致文件丢失）
    """
    try:
        old_path_obj = Path(old_path).resolve()
        new_path_obj = old_path_obj.parent  / new_filename
        
        # 基础验证 
        if not old_path_obj.exists(): 
            raise FileNotFoundError(f"源文件不存在: {old_path_obj}")
        if not old_path_obj.is_file(): 
            raise IsADirectoryError(f"源路径是目录而非文件: {old_path_obj}")
            
        # 处理目标文件已存在的情况 
        if new_path_obj.exists(): 
            print(f"警告: 目标文件已存在，正在覆盖 {new_path_obj}")
            if new_path_obj.is_dir(): 
                raise IsADirectoryError(f"不能覆盖目录: {new_path_obj}")
            new_path_obj.unlink()   # 删除现有文件
            
        # 执行原子化重命名（推荐方案）
        old_path_obj.replace(new_path_obj)   # 等同于os.replace() 
        
        print(f"成功: {old_path_obj} -> {new_path_obj}")
        return True
        
    except Exception as e:
        print(f"操作失败: {type(e).__name__} - {str(e)}")
        return False


 


 
def main():
    parser = argparse.ArgumentParser(description="vot config 文件生成")
    parser.add_argument("output_file",   help="输出文件路径")
    args = parser.parse_args()

    current_path = os.path.abspath(__file__) 
    print(f"当前脚本路径：{current_path},  {Path(current_path).parent }") 

    try:
       
        usr_cfg_data = generate_user_cfg()
         
        # # 计算CRC
        # crc = CRC16Modbus.calculate(firm_data) 
        # print(f"CRC16-Modbus: 0x{crc:04X}")
 

        output = args.output_file  if args.output_file  else "vot_config.bin" 
        with open(output, 'wb') as f:
            if output: 
                f.write(usr_cfg_data )  # 全量写入 
       
        print(f"处理完成 → {output}")
 
    except Exception as e:
        print(f"错误: {e}", file=sys.stderr) 
        sys.exit(1) 

 
if __name__ == '__main__':
    main()

# usages:
# python3 generate_vot_config.py  ts_vision.uimg  ts_vision_padded.bin  --mode=standard

