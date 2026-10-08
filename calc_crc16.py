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
    cfg.cfg_reserved_bytes = 0;
    
    current_path = os.path.abspath(__file__) 
    #base_path = Path(current_path).parent 
    base_path = Path(current_path).parent
    #print(f"当前脚本路径：{current_path},  {base_path}") 


    usrcfg_data=bytes(0)  
    # 初始化节点数组 
    file_name = base_path / "isp_outdoor.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp1_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp1_data 
                cfg.cfg_total_bytes += len(isp1_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[cfg.cfg_node_nums]  = UserCfgNodeS(b"isp_outdoor", 0, 4096)
                cfg.cfg_node_nums  += 1
    except FileNotFoundError:
        pass  # 静默跳过 


    file_name = base_path / "isp_indoor.bin" 
    try:
        if os.path.isfile(file_name)  and not os.path.islink(file_name): 
            print(f"file_name: {file_name}")
            with open(file_name,  'rb') as f1:
                isp2_data = pad_file_to_size(file_name, 4096)
                usrcfg_data += isp2_data
                cfg.cfg_total_bytes += len(isp2_data)
                print(f"cfg.cfg_total_bytes: {cfg.cfg_total_bytes}") 
                cfg.cfg_node_array[1]  = UserCfgNodeS(b"isp_indoor", 4096, 4096)
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

def get_version_from_file(file_path: str = 'version.txt')  -> Optional[str]:
    """
    从指定文件中提取VERSION变量

    参数:
        file_path (str): 版本文件路径，默认为'version.txt' 
        
    返回:
        str: 提取到的版本号，如果失败则返回None
        
    异常:
        FileNotFoundError: 当文件不存在时抛出
        ValueError: 当文件格式不符合预期时抛出 
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read() 
            
        # 支持多种格式匹配：VERSION = "1.0" 或 VERSION='1.0' 或 VERSION = 1.0 
        match = re.search( 
            r'VERSION\s*=\s*["\']?([^"\'\s]+)["\']?', 
            content,
            re.IGNORECASE
        )
        
        if not match:
            raise ValueError(f"在 {file_path} 中未找到有效的VERSION定义")
            
        return match.group(1) 
        
    except FileNotFoundError:
        raise FileNotFoundError(f"版本文件不存在: {file_path}")
    except UnicodeDecodeError:
        raise ValueError(f"文件编码不是UTF-8: {file_path}")
    except Exception as e:
        raise ValueError(f"解析版本失败: {str(e)}")



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


def date_to_bcd() -> bytes:
    """
    将当前日期转换为BCD编码的4字节（兼容嵌入式系统）
    输出格式: [世纪年, 年份, 月份, 日]
    """
    now = datetime.now() 
    return bytes([
        ((now.year  // 100) // 10 << 4) | ((now.year  // 100) % 10),  # 世纪年BCD 
        ((now.year  % 100) // 10 << 4) | ((now.year  % 100) % 10),    # 年份BCD 
        ((now.month)  // 10 << 4) | ((now.month)  % 10),              # 月份BCD
        ((now.day)  // 10 << 4) | ((now.day)  % 10)                   # 日BCD
    ])
 
# 测试输出（2025-09-09）
#print(date_to_bcd().hex(' '))  # 输出: 20 25 09 09 

class CRC16Modbus:
    """动态计算的CRC16-Modbus类（无查表依赖）"""
    @staticmethod 
    def _update_crc(crc: int, byte: int) -> int:
        """单字节CRC计算核心"""
        crc ^= byte
        for _ in range(8):
            crc = (crc >> 1) ^ (0xA001 if (crc & 1) else 0x0000)
        return crc & 0xFFFF  # 确保结果为16位 
 
    @classmethod
    def calculate(cls, data: bytes) -> int:
        """计算完整数据的CRC16-Modbus值""" 
        crc = 0xFFFF  # Modbus初始值
        for byte in data:
            crc = cls._update_crc(crc, byte)
        return crc


def pad_to_size(input_bin: str, output_bin: str, target_kb: int = 512) -> bytes: 
    """SPI Flash专用填充函数（含CRC和元数据）"""
    with open(input_bin, 'rb') as f:
        data = f.read() 
    
    # 计算填充大小（预留16字节元数据区）
    pad_size = target_kb * 1024 - len(data) - 16
    assert pad_size >= 0, "固件超过目标大小"
    
    # 构造元数据（CRC32 + 日期BCD + 版本）
    metadata = ( 
        crc32(data).to_bytes(4, 'big') +          # 4字节CRC
        bytes([0x20, 0x25, 0x09, 0x09]) +         # 2025-09-09 
        b'FW_2025\x00'                            # 8字节版本
    )
    
    # 生成优化填充模式（0xFF与0x55AA混合）
    padding = bytes([0xFF if i % 2 else 0x55 for i in range(pad_size)])
    
    with open(output_bin, 'wb') as f:
        f.write(data  + padding + metadata)  # 固件+填充+元数据




def generate_padding(crc: int, mode: str = 'standard') -> bytes:
    """
    生成64字节结构化填充数据 
    数据结构：
    | 0-1字节 | 2-5字节       | 6-30字节       | 31-63字节    |
    |---------|---------------|----------------|--------------|
    | CRC16   | 日期(BCD编码) | 版本(ASCII)    | 动态填充     |
 
    参数：
        crc: CRC16校验值（2字节）
        mode: 动态填充模式（standard/random/secure）
    """
    # 1. 从version.txt 读取VERSION变量
    try:
        with open('version.txt',  'r') as f:
            content = f.read() 
            version = re.search(r'VERSION\s*=\s*["\'](.+?)["\']',  content).group(1)
    except Exception as e:
        raise ValueError(f"无法从version.txt 读取VERSION变量: {e}")
 
    # 2. CRC部分（大端序）
    crc_bytes = crc.to_bytes(2,  'big')
 
    # 3. 当前日期BCD编码（2025-09-09 → 0x20 0x25 0x09 0x09）
    date_bytes = date_to_bcd()
 
    # 4. 版本信息（25字节ASCII，右侧空格填充）
    version_bytes = version.ljust(25).encode('ascii')[:25]   # 截断超长部分
    print(f"version: {version}")
 
    # 5. 动态填充部分（1字节）+ (32字节)
    if mode == 'standard':
        dynamic_part = bytes([0xAA] * 33)
    elif mode == 'random':
        dynamic_part = bytes([0x1F, 0xB2, 0x4D, 0x88, 0x63, 0xE7, 0x09, 0x5C ] * 4) + bytes([0xAA]) 
    elif mode == 'secure':
        dynamic_part = bytes([0x55, 0xAA, 0x33, 0xCC, 0x66, 0x99, 0x12, 0x34] * 4) + bytes([0xAA])   
    else:
        raise ValueError(f"无效填充模式: {mode}（支持standard/random/secure）")
 
    # 组合所有部分（总长度=2+4+25+1+32=64字节）
    return crc_bytes + date_bytes + version_bytes + dynamic_part

 
def main():
    parser = argparse.ArgumentParser(description="独立CRC16-Modbus计算工具")
    parser.add_argument("input_file",  help="输入固件文件路径")
    parser.add_argument("output_file",  nargs='?', help="输出文件路径（可选, 默认追加到输入文件）")
    parser.add_argument("--output_file_prefix",  default='VOT_ASIC_V', help="输出固件名的前缀")
    parser.add_argument("--mode",  choices=['standard', 'random', 'secure'], default='standard',
                       help="填充模式（repeat/date/random）")
    parser.add_argument("--isp_cfg",  nargs='?', help="用户自定义配置 （可选）")
    parser.add_argument("--target_kb",  type=int, required=True, help="填充的目标大小,单位KB")  # 关键修复 
    #parser.add_argument("--target_kb",  type=int, default=512, help="填充目标大小KB，默认512") 
    args = parser.parse_args()

    current_path = os.path.abspath(__file__) 
    print(f"当前脚本路径：{current_path},  {Path(current_path).parent }") 

    try:
        if not os.path.exists(args.input_file): 
            parser.error(f" 输入文件不存在: {args.input_file}") 

        if os.path.getsize(args.input_file)  > args.target_kb  * 1024:
            raise ValueError("输入文件已超过目标大小")

        # 读取固件数据
        with open(args.input_file,  'rb') as f:
            img_data = f.read() 

        # 计算填充大小（预留32字节元数据区）
        fill_pad_size = args.target_kb  * 1024 - len(img_data) 
        assert fill_pad_size >= 0, f"固件大小{len(img_data)/1024:.2f}KB已超过目标{args.target_kb}KB" 
        print(f"目标大小: {args.target_kb}KB  | 需填充: {fill_pad_size}字节")

        # 生成优化填充模式（0xFF与0x55混合）
        fill_padding = bytes([0xFF if i % 2 else 0x55 for i in range(fill_pad_size)])

        firm_data = img_data + fill_padding  
#        if args.isp_cfg :
           #print(f"isp_cfg: {args.isp_cfg}")
           #with open(args.isp_cfg,  'rb') as f1:
                #isp_data = pad_file_to_size(args.isp_cfg, 4096)
                #usr_cfg_header = generate_user_cfg_header()
                #firm_data = img_data + fill_padding + usr_cfg_header + isp_data

        # usr_cfg_data = generate_user_cfg()
        # firm_data = img_data + fill_padding + usr_cfg_data
         

        # 计算CRC
        crc = CRC16Modbus.calculate(firm_data) 
        print(f"CRC16-Modbus: 0x{crc:04X}")
 
        # 生成并写入填充
        padding = generate_padding(crc, args.mode) 
        # 方法1：Python 3.8+专用（推荐）
        try:
            # 统一兼容的十六进制格式化 
           hex_str = '  '.join(f"{b:02X}" for b in padding)  # 双空格分隔 
           print(f"填充数据（{len(padding)}字节）: {hex_str}")
        except TypeError:
        # 方法2：全版本兼容 
            hex_str = ' '.join(f"{b:02X}" for b in padding)
            #hex_str = ','.join(f"0x{b:02X}" for b in padding)
            print(f"adaptive 填充数据（64字节）: {hex_str}")

        output = args.output_file  if args.output_file  else args.input_file 
        with open(output, 'ab' if not args.output_file  else 'wb') as f:
            if args.output_file: 
                f.write(firm_data + padding  )  # 全量写入 
            else:
                f.write( padding  )          # 追加模式
 
        print(f"处理完成 → {output}")
 
    except Exception as e:
        print(f"错误: {e}", file=sys.stderr) 
        sys.exit(1) 

    try:
        version = get_version_from_file()
        print(f"当前版本: {version}")
        old_name = args.output_file
        old_path = Path(old_name)
        new_name = args.output_file_prefix + version + ".bin"
        new_filename = old_path.parent  / new_name
       
        rename_file_with_overwrite(old_name, new_name)
        # print(f"old_name: {old_name},new_filename:{new_filename} ")
    except Exception as e:
        print(f"获取版本失败: {e}")

 
if __name__ == '__main__':
    main()

# usages:
# python3 calc_crc16.py  ts_vision.uimg  ts_vision_padded.bin  --mode=standard

