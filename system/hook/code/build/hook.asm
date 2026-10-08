
build/hook.elf：     文件格式 elf32-littleriscv


Disassembly of section .text:

02200000 <bootup_hook>:
 2200000:	022027b7          	lui	a5,0x2202
 2200004:	02202737          	lui	a4,0x2202
 2200008:	cec78693          	addi	a3,a5,-788 # 2201cec <sst_bt_ctx>
 220000c:	d5870713          	addi	a4,a4,-680 # 2201d58 <__hook_end>
 2200010:	8f15                	sub	a4,a4,a3
 2200012:	9b71                	andi	a4,a4,-4
 2200014:	1141                	addi	sp,sp,-16
 2200016:	0711                	addi	a4,a4,4
 2200018:	c606                	sw	ra,12(sp)
 220001a:	9736                	add	a4,a4,a3
 220001c:	cec78793          	addi	a5,a5,-788
 2200020:	5847d00b          	swia	zero,(a5),4,0
 2200024:	fee79ee3          	bne	a5,a4,2200020 <bootup_hook+0x20>
 2200028:	4685                	li	a3,1
 220002a:	0fa00613          	li	a2,250
 220002e:	06400593          	li	a1,100
 2200032:	0c800513          	li	a0,200
 2200036:	2879                	jal	22000d4 <hook_clkm_init>
 2200038:	4581                	li	a1,0
 220003a:	4511                	li	a0,4
 220003c:	604000e7          	jalr	1540(zero) # 0 <__DYNAMIC>
 2200040:	28b9                	jal	220009e <soc_init>
 2200042:	0fa00693          	li	a3,250
 2200046:	4609                	li	a2,2
 2200048:	4585                	li	a1,1
 220004a:	4505                	li	a0,1
 220004c:	35f000ef          	jal	ra,2200baa <hook_ddr_init>
 2200050:	6b2000e7          	jalr	1714(zero) # 0 <__DYNAMIC>
 2200054:	74e010ef          	jal	ra,22017a2 <hook_boot_app>
 2200058:	40b2                	lw	ra,12(sp)
 220005a:	4501                	li	a0,0
 220005c:	0141                	addi	sp,sp,16
 220005e:	8082                	ret
	...

02200062 <soc_get_ddr_bank_regbase>:
 2200062:	4785                	li	a5,1
 2200064:	00a7f463          	bgeu	a5,a0,220006c <soc_get_ddr_bank_regbase+0xa>
 2200068:	4501                	li	a0,0
 220006a:	8082                	ret
 220006c:	022027b7          	lui	a5,0x2202
 2200070:	cc478793          	addi	a5,a5,-828 # 2201cc4 <__hook_text_end>
 2200074:	44a7c50b          	lrw	a0,a5,a0,2
 2200078:	8082                	ret

0220007a <soc_get_osc_clk>:
 220007a:	016e3537          	lui	a0,0x16e3
 220007e:	60050513          	addi	a0,a0,1536 # 16e3600 <__text_end+0x16df5f8>
 2200082:	8082                	ret

02200084 <soc_get_uart_regbase>:
 2200084:	4791                	li	a5,4
 2200086:	00a7e963          	bltu	a5,a0,2200098 <soc_get_uart_regbase+0x14>
 220008a:	022027b7          	lui	a5,0x2202
 220008e:	ccc78793          	addi	a5,a5,-820 # 2201ccc <gau32_uart_regs>
 2200092:	44a7c50b          	lrw	a0,a5,a0,2
 2200096:	8082                	ret
 2200098:	4501                	li	a0,0
 220009a:	8082                	ret

0220009c <soc_uart_set_mux>:
 220009c:	8082                	ret

0220009e <soc_init>:
 220009e:	008407b7          	lui	a5,0x840
 22000a2:	17fd                	addi	a5,a5,-1
 22000a4:	3b079073          	csrw	pmpaddr0,a5
 22000a8:	3a0027f3          	csrr	a5,pmpcfg0
 22000ac:	09b7e793          	ori	a5,a5,155
 22000b0:	3a079073          	csrw	pmpcfg0,a5
 22000b4:	001607b7          	lui	a5,0x160
 22000b8:	1307a703          	lw	a4,304(a5) # 160130 <__text_end+0x15c128>
 22000bc:	76fd                	lui	a3,0xfffff
 22000be:	16fd                	addi	a3,a3,-1
 22000c0:	8f75                	and	a4,a4,a3
 22000c2:	12e7a823          	sw	a4,304(a5)
 22000c6:	1507a703          	lw	a4,336(a5)
 22000ca:	6695                	lui	a3,0x5
 22000cc:	8f55                	or	a4,a4,a3
 22000ce:	14e7a823          	sw	a4,336(a5)
 22000d2:	8082                	ret

022000d4 <hook_clkm_init>:
 22000d4:	1101                	addi	sp,sp,-32
 22000d6:	cc22                	sw	s0,24(sp)
 22000d8:	ca26                	sw	s1,20(sp)
 22000da:	ce06                	sw	ra,28(sp)
 22000dc:	842a                	mv	s0,a0
 22000de:	84ae                	mv	s1,a1
 22000e0:	14068563          	beqz	a3,220022a <hook_clkm_init+0x156>
 22000e4:	00a51793          	slli	a5,a0,0xa
 22000e8:	00130737          	lui	a4,0x130
 22000ec:	0187e793          	ori	a5,a5,24
 22000f0:	c31c                	sw	a5,0(a4)
 22000f2:	6785                	lui	a5,0x1
 22000f4:	11278793          	addi	a5,a5,274 # 1112 <lib_sfc_write_en+0x12>
 22000f8:	c71c                	sw	a5,8(a4)
 22000fa:	47c1                	li	a5,16
 22000fc:	c75c                	sw	a5,12(a4)
 22000fe:	25800813          	li	a6,600
 2200102:	85b2                	mv	a1,a2
 2200104:	4505                	li	a0,1
 2200106:	48c1                	li	a7,16
 2200108:	4321                	li	t1,8
 220010a:	87ae                	mv	a5,a1
 220010c:	4705                	li	a4,1
 220010e:	0001                	nop
 2200110:	0107fb63          	bgeu	a5,a6,2200126 <hook_clkm_init+0x52>
 2200114:	0705                	addi	a4,a4,1
 2200116:	97ae                	add	a5,a5,a1
 2200118:	ff171ce3          	bne	a4,a7,2200110 <hook_clkm_init+0x3c>
 220011c:	0505                	addi	a0,a0,1
 220011e:	95b2                	add	a1,a1,a2
 2200120:	fe6515e3          	bne	a0,t1,220010a <hook_clkm_init+0x36>
 2200124:	4501                	li	a0,0
 2200126:	00130637          	lui	a2,0x130
 220012a:	0522                	slli	a0,a0,0x8
 220012c:	6305                	lui	t1,0x1
 220012e:	4e0c                	lw	a1,24(a2)
 2200130:	8f49                	or	a4,a4,a0
 2200132:	f0f30513          	addi	a0,t1,-241 # f0f <lib_sfc_write_sr+0x3f>
 2200136:	8f69                	and	a4,a4,a0
 2200138:	757d                	lui	a0,0xfffff
 220013a:	00100837          	lui	a6,0x100
 220013e:	0f050513          	addi	a0,a0,240 # fffff0f0 <__end+0xfddc0738>
 2200142:	8d6d                	and	a0,a0,a1
 2200144:	01062883          	lw	a7,16(a2) # 130010 <__text_end+0x12c008>
 2200148:	f0080e13          	addi	t3,a6,-256 # fff00 <__text_end+0xfbef8>
 220014c:	07a2                	slli	a5,a5,0x8
 220014e:	8f49                	or	a4,a4,a0
 2200150:	01c7f7b3          	and	a5,a5,t3
 2200154:	fff00537          	lui	a0,0xfff00
 2200158:	f3f80813          	addi	a6,a6,-193
 220015c:	0187e793          	ori	a5,a5,24
 2200160:	0c050513          	addi	a0,a0,192 # fff000c0 <__end+0xfdcc1708>
 2200164:	0107f7b3          	and	a5,a5,a6
 2200168:	00a8f533          	and	a0,a7,a0
 220016c:	7841                	lui	a6,0xffff0
 220016e:	8fc9                	or	a5,a5,a0
 2200170:	3c07370b          	extu	a4,a4,15,0
 2200174:	0105f5b3          	and	a1,a1,a6
 2200178:	8f4d                	or	a4,a4,a1
 220017a:	ca1c                	sw	a5,16(a2)
 220017c:	47c1                	li	a5,16
 220017e:	ce5c                	sw	a5,28(a2)
 2200180:	ce18                	sw	a4,24(a2)
 2200182:	ea89                	bnez	a3,2200194 <hook_clkm_init+0xc0>
 2200184:	67b9                	lui	a5,0xe
 2200186:	81878793          	addi	a5,a5,-2024 # d818 <__text_end+0x9810>
 220018a:	d21c                	sw	a5,32(a2)
 220018c:	11230313          	addi	t1,t1,274
 2200190:	02662423          	sw	t1,40(a2)
 2200194:	4741                	li	a4,16
 2200196:	001307b7          	lui	a5,0x130
 220019a:	d7d8                	sw	a4,44(a5)
 220019c:	dfd8                	sw	a4,60(a5)
 220019e:	c7f8                	sw	a4,76(a5)
 22001a0:	cff8                	sw	a4,92(a5)
 22001a2:	6741                	lui	a4,0x10
 22001a4:	0705                	addi	a4,a4,1
 22001a6:	001306b7          	lui	a3,0x130
 22001aa:	a011                	j	22001ae <hook_clkm_init+0xda>
 22001ac:	cb5d                	beqz	a4,2200262 <hook_clkm_init+0x18e>
 22001ae:	52fc                	lw	a5,100(a3)
 22001b0:	177d                	addi	a4,a4,-1
 22001b2:	07f7f793          	andi	a5,a5,127
 22001b6:	dbfd                	beqz	a5,22001ac <hook_clkm_init+0xd8>
 22001b8:	02945733          	divu	a4,s0,s1
 22001bc:	0014d813          	srli	a6,s1,0x1
 22001c0:	010436b3          	sltu	a3,s0,a6
 22001c4:	009434b3          	sltu	s1,s0,s1
 22001c8:	0014c493          	xori	s1,s1,1
 22001cc:	001307b7          	lui	a5,0x130
 22001d0:	80000637          	lui	a2,0x80000
 22001d4:	03f00593          	li	a1,63
 22001d8:	0016c693          	xori	a3,a3,1
 22001dc:	d3ac                	sw	a1,96(a5)
 22001de:	02faf5b7          	lui	a1,0x2faf
 22001e2:	08058593          	addi	a1,a1,128 # 2faf080 <__end+0xd706c8>
 22001e6:	4505                	li	a0,1
 22001e8:	03045433          	divu	s0,s0,a6
 22001ec:	8f05                	sub	a4,a4,s1
 22001ee:	8f51                	or	a4,a4,a2
 22001f0:	d7f8                	sw	a4,108(a5)
 22001f2:	8c15                	sub	s0,s0,a3
 22001f4:	8c51                	or	s0,s0,a2
 22001f6:	dbe0                	sw	s0,116(a5)
 22001f8:	5fb8                	lw	a4,120(a5)
 22001fa:	00476713          	ori	a4,a4,4
 22001fe:	dfb8                	sw	a4,120(a5)
 2200200:	6765                	lui	a4,0x19
 2200202:	cfc70713          	addi	a4,a4,-772 # 18cfc <__text_end+0x14cf4>
 2200206:	0ce7a223          	sw	a4,196(a5) # 1300c4 <__text_end+0x12c0bc>
 220020a:	30600713          	li	a4,774
 220020e:	0ce7a423          	sw	a4,200(a5)
 2200212:	577d                	li	a4,-1
 2200214:	0ce7a623          	sw	a4,204(a5)
 2200218:	4705                	li	a4,1
 220021a:	0ce7a823          	sw	a4,208(a5)
 220021e:	4462                	lw	s0,24(sp)
 2200220:	40f2                	lw	ra,28(sp)
 2200222:	44d2                	lw	s1,20(sp)
 2200224:	6105                	addi	sp,sp,32
 2200226:	19a00067          	jr	410(zero) # 0 <__DYNAMIC>
 220022a:	02202537          	lui	a0,0x2202
 220022e:	b8450513          	addi	a0,a0,-1148 # 2201b84 <__cmdl_end>
 2200232:	c636                	sw	a3,12(sp)
 2200234:	c432                	sw	a2,8(sp)
 2200236:	fde02097          	auipc	ra,0xfde02
 220023a:	5f4080e7          	jalr	1524(ra) # 282a <printf>
 220023e:	00941793          	slli	a5,s0,0x9
 2200242:	00130737          	lui	a4,0x130
 2200246:	0187e793          	ori	a5,a5,24
 220024a:	c31c                	sw	a5,0(a4)
 220024c:	6785                	lui	a5,0x1
 220024e:	11178793          	addi	a5,a5,273 # 1111 <lib_sfc_write_en+0x11>
 2200252:	c71c                	sw	a5,8(a4)
 2200254:	47c1                	li	a5,16
 2200256:	c75c                	sw	a5,12(a4)
 2200258:	4622                	lw	a2,8(sp)
 220025a:	46b2                	lw	a3,12(sp)
 220025c:	19000813          	li	a6,400
 2200260:	b54d                	j	2200102 <hook_clkm_init+0x2e>
 2200262:	022025b7          	lui	a1,0x2202
 2200266:	b9458593          	addi	a1,a1,-1132 # 2201b94 <__cmdl_end+0x10>
 220026a:	4505                	li	a0,1
 220026c:	fde02097          	auipc	ra,0xfde02
 2200270:	632080e7          	jalr	1586(ra) # 289e <ts_log>
 2200274:	b791                	j	22001b8 <hook_clkm_init+0xe4>
	...

02200278 <psram_x8_32Mb_T200_init.constprop.0>:
 2200278:	7139                	addi	sp,sp,-64
 220027a:	dc22                	sw	s0,56(sp)
 220027c:	da26                	sw	s1,52(sp)
 220027e:	d84a                	sw	s2,48(sp)
 2200280:	d64e                	sw	s3,44(sp)
 2200282:	d452                	sw	s4,40(sp)
 2200284:	d256                	sw	s5,36(sp)
 2200286:	d05a                	sw	s6,32(sp)
 2200288:	ce5e                	sw	s7,28(sp)
 220028a:	cc62                	sw	s8,24(sp)
 220028c:	ca66                	sw	s9,20(sp)
 220028e:	c66e                	sw	s11,12(sp)
 2200290:	de06                	sw	ra,60(sp)
 2200292:	c86a                	sw	s10,16(sp)
 2200294:	02000793          	li	a5,32
 2200298:	c11c                	sw	a5,0(a0)
 220029a:	c11c                	sw	a5,0(a0)
 220029c:	e060a7b7          	lui	a5,0xe060a
 22002a0:	02078793          	addi	a5,a5,32 # e060a020 <__end+0xde3cb668>
 22002a4:	c15c                	sw	a5,4(a0)
 22002a6:	6785                	lui	a5,0x1
 22002a8:	e6678793          	addi	a5,a5,-410 # e66 <loader_try_next+0x11e>
 22002ac:	cd5c                	sw	a5,28(a0)
 22002ae:	000727b7          	lui	a5,0x72
 22002b2:	73378793          	addi	a5,a5,1843 # 72733 <__text_end+0x6e72b>
 22002b6:	d11c                	sw	a5,32(a0)
 22002b8:	79e1                	lui	s3,0xffff8
 22002ba:	6909                	lui	s2,0x2
 22002bc:	8caa                	mv	s9,a0
 22002be:	02052223          	sw	zero,36(a0)
 22002c2:	10450c13          	addi	s8,a0,260
 22002c6:	10c50b93          	addi	s7,a0,268
 22002ca:	11050b13          	addi	s6,a0,272
 22002ce:	11c50a93          	addi	s5,a0,284
 22002d2:	12050a13          	addi	s4,a0,288
 22002d6:	11450d93          	addi	s11,a0,276
 22002da:	10850413          	addi	s0,a0,264
 22002de:	02800493          	li	s1,40
 22002e2:	08098993          	addi	s3,s3,128 # ffff8080 <__end+0xfddb96c8>
 22002e6:	22090913          	addi	s2,s2,544 # 2220 <hbp_lib_exec+0x474>
 22002ea:	000c2023          	sw	zero,0(s8)
 22002ee:	000ba783          	lw	a5,0(s7)
 22002f2:	4d29                	li	s10,10
 22002f4:	f007f793          	andi	a5,a5,-256
 22002f8:	8fc5                	or	a5,a5,s1
 22002fa:	00fba023          	sw	a5,0(s7)
 22002fe:	000b2783          	lw	a5,0(s6)
 2200302:	0137f7b3          	and	a5,a5,s3
 2200306:	0127e7b3          	or	a5,a5,s2
 220030a:	00fb2023          	sw	a5,0(s6)
 220030e:	000aa783          	lw	a5,0(s5)
 2200312:	f007f793          	andi	a5,a5,-256
 2200316:	8fc5                	or	a5,a5,s1
 2200318:	00faa023          	sw	a5,0(s5)
 220031c:	000a2783          	lw	a5,0(s4)
 2200320:	0137f7b3          	and	a5,a5,s3
 2200324:	0127e7b3          	or	a5,a5,s2
 2200328:	00fa2023          	sw	a5,0(s4)
 220032c:	4785                	li	a5,1
 220032e:	00fc2023          	sw	a5,0(s8)
 2200332:	401c                	lw	a5,0(s0)
 2200334:	8b85                	andi	a5,a5,1
 2200336:	ef81                	bnez	a5,220034e <psram_x8_32Mb_T200_init.constprop.0+0xd6>
 2200338:	4529                	li	a0,10
 220033a:	fde02097          	auipc	ra,0xfde02
 220033e:	80c080e7          	jalr	-2036(ra) # 1b46 <dly_lib_delay_us>
 2200342:	401c                	lw	a5,0(s0)
 2200344:	1d7d                	addi	s10,s10,-1
 2200346:	8b85                	andi	a5,a5,1
 2200348:	e399                	bnez	a5,220034e <psram_x8_32Mb_T200_init.constprop.0+0xd6>
 220034a:	fe0d17e3          	bnez	s10,2200338 <psram_x8_32Mb_T200_init.constprop.0+0xc0>
 220034e:	000da783          	lw	a5,0(s11)
 2200352:	4751                	li	a4,20
 2200354:	3c87b78b          	extu	a5,a5,15,8
 2200358:	00f76863          	bltu	a4,a5,2200368 <psram_x8_32Mb_T200_init.constprop.0+0xf0>
 220035c:	02848493          	addi	s1,s1,40
 2200360:	0c800793          	li	a5,200
 2200364:	f8f493e3          	bne	s1,a5,22002ea <psram_x8_32Mb_T200_init.constprop.0+0x72>
 2200368:	040007b7          	lui	a5,0x4000
 220036c:	70178793          	addi	a5,a5,1793 # 4000701 <__end+0x1dc1d49>
 2200370:	002016b7          	lui	a3,0x201
 2200374:	02fca423          	sw	a5,40(s9)
 2200378:	11168693          	addi	a3,a3,273 # 201111 <__text_end+0x1fd109>
 220037c:	002007b7          	lui	a5,0x200
 2200380:	12dca823          	sw	a3,304(s9)
 2200384:	11178793          	addi	a5,a5,273 # 200111 <__text_end+0x1fc109>
 2200388:	12fcaa23          	sw	a5,308(s9)
 220038c:	12dcac23          	sw	a3,312(s9)
 2200390:	12fcae23          	sw	a5,316(s9)
 2200394:	14fca023          	sw	a5,320(s9)
 2200398:	14fca223          	sw	a5,324(s9)
 220039c:	14fca423          	sw	a5,328(s9)
 22003a0:	14fca623          	sw	a5,332(s9)
 22003a4:	14fca823          	sw	a5,336(s9)
 22003a8:	14fcaa23          	sw	a5,340(s9)
 22003ac:	14fcac23          	sw	a5,344(s9)
 22003b0:	14fcae23          	sw	a5,348(s9)
 22003b4:	16fca023          	sw	a5,352(s9)
 22003b8:	16fca223          	sw	a5,356(s9)
 22003bc:	16fca423          	sw	a5,360(s9)
 22003c0:	16fca623          	sw	a5,364(s9)
 22003c4:	16fca823          	sw	a5,368(s9)
 22003c8:	16fcaa23          	sw	a5,372(s9)
 22003cc:	16fcac23          	sw	a5,376(s9)
 22003d0:	16fcae23          	sw	a5,380(s9)
 22003d4:	18fca023          	sw	a5,384(s9)
 22003d8:	20400793          	li	a5,516
 22003dc:	10fca023          	sw	a5,256(s9)
 22003e0:	4785                	li	a5,1
 22003e2:	12fca623          	sw	a5,300(s9)
 22003e6:	120ca623          	sw	zero,300(s9)
 22003ea:	120ca623          	sw	zero,300(s9)
 22003ee:	120ca623          	sw	zero,300(s9)
 22003f2:	120ca623          	sw	zero,300(s9)
 22003f6:	12fca623          	sw	a5,300(s9)
 22003fa:	09600513          	li	a0,150
 22003fe:	fde01097          	auipc	ra,0xfde01
 2200402:	748080e7          	jalr	1864(ra) # 1b46 <dly_lib_delay_us>
 2200406:	000ca423          	sw	zero,8(s9)
 220040a:	000ca623          	sw	zero,12(s9)
 220040e:	03000793          	li	a5,48
 2200412:	00fcac23          	sw	a5,24(s9)
 2200416:	03100793          	li	a5,49
 220041a:	018c8413          	addi	s0,s9,24
 220041e:	00fcac23          	sw	a5,24(s9)
 2200422:	0001                	nop
 2200424:	401c                	lw	a5,0(s0)
 2200426:	1007f793          	andi	a5,a5,256
 220042a:	dfed                	beqz	a5,2200424 <psram_x8_32Mb_T200_init.constprop.0+0x1ac>
 220042c:	014ca783          	lw	a5,20(s9)
 2200430:	05f00693          	li	a3,95
 2200434:	3c07b78b          	extu	a5,a5,15,0
 2200438:	12d79f63          	bne	a5,a3,2200576 <psram_x8_32Mb_T200_init.constprop.0+0x2fe>
 220043c:	4785                	li	a5,1
 220043e:	00fca423          	sw	a5,8(s9)
 2200442:	000ca623          	sw	zero,12(s9)
 2200446:	03000793          	li	a5,48
 220044a:	00fcac23          	sw	a5,24(s9)
 220044e:	03100793          	li	a5,49
 2200452:	00fcac23          	sw	a5,24(s9)
 2200456:	0001                	nop
 2200458:	401c                	lw	a5,0(s0)
 220045a:	1007f793          	andi	a5,a5,256
 220045e:	dfed                	beqz	a5,2200458 <psram_x8_32Mb_T200_init.constprop.0+0x1e0>
 2200460:	014ca783          	lw	a5,20(s9)
 2200464:	46bd                	li	a3,15
 2200466:	3c07b78b          	extu	a5,a5,15,0
 220046a:	14d79763          	bne	a5,a3,22005b8 <psram_x8_32Mb_T200_init.constprop.0+0x340>
 220046e:	010007b7          	lui	a5,0x1000
 2200472:	00fca423          	sw	a5,8(s9)
 2200476:	000ca623          	sw	zero,12(s9)
 220047a:	03000793          	li	a5,48
 220047e:	00fcac23          	sw	a5,24(s9)
 2200482:	03100793          	li	a5,49
 2200486:	00fcac23          	sw	a5,24(s9)
 220048a:	0001                	nop
 220048c:	401c                	lw	a5,0(s0)
 220048e:	1007f793          	andi	a5,a5,256
 2200492:	dfed                	beqz	a5,220048c <psram_x8_32Mb_T200_init.constprop.0+0x214>
 2200494:	014ca783          	lw	a5,20(s9)
 2200498:	66a5                	lui	a3,0x9
 220049a:	f1f68693          	addi	a3,a3,-225 # 8f1f <__text_end+0x4f17>
 220049e:	3c07b60b          	extu	a2,a5,15,0
 22004a2:	10d61063          	bne	a2,a3,22005a2 <psram_x8_32Mb_T200_init.constprop.0+0x32a>
 22004a6:	f047f793          	andi	a5,a5,-252
 22004aa:	0107e793          	ori	a5,a5,16
 22004ae:	00fca823          	sw	a5,16(s9)
 22004b2:	02000793          	li	a5,32
 22004b6:	00fcac23          	sw	a5,24(s9)
 22004ba:	02100793          	li	a5,33
 22004be:	00fcac23          	sw	a5,24(s9)
 22004c2:	0001                	nop
 22004c4:	401c                	lw	a5,0(s0)
 22004c6:	1007f793          	andi	a5,a5,256
 22004ca:	dfed                	beqz	a5,22004c4 <psram_x8_32Mb_T200_init.constprop.0+0x24c>
 22004cc:	01cca783          	lw	a5,28(s9)
 22004d0:	001d06b7          	lui	a3,0x1d0
 22004d4:	f007f793          	andi	a5,a5,-256
 22004d8:	0667e793          	ori	a5,a5,102
 22004dc:	00fcae23          	sw	a5,28(s9)
 22004e0:	03000793          	li	a5,48
 22004e4:	ce9c                	sw	a5,24(a3)
 22004e6:	03100793          	li	a5,49
 22004ea:	ce9c                	sw	a5,24(a3)
 22004ec:	401c                	lw	a5,0(s0)
 22004ee:	1007f793          	andi	a5,a5,256
 22004f2:	dfed                	beqz	a5,22004ec <psram_x8_32Mb_T200_init.constprop.0+0x274>
 22004f4:	014ca783          	lw	a5,20(s9)
 22004f8:	4689                	li	a3,2
 22004fa:	838d                	srli	a5,a5,0x3
 22004fc:	8bf9                	andi	a5,a5,30
 22004fe:	08d79763          	bne	a5,a3,220058c <psram_x8_32Mb_T200_init.constprop.0+0x314>
 2200502:	010007b7          	lui	a5,0x1000
 2200506:	0785                	addi	a5,a5,1
 2200508:	00fca423          	sw	a5,8(s9)
 220050c:	000ca623          	sw	zero,12(s9)
 2200510:	03000793          	li	a5,48
 2200514:	00fcac23          	sw	a5,24(s9)
 2200518:	03100793          	li	a5,49
 220051c:	00fcac23          	sw	a5,24(s9)
 2200520:	401c                	lw	a5,0(s0)
 2200522:	1007f793          	andi	a5,a5,256
 2200526:	dfed                	beqz	a5,2200520 <psram_x8_32Mb_T200_init.constprop.0+0x2a8>
 2200528:	014ca783          	lw	a5,20(s9)
 220052c:	66c1                	lui	a3,0x10
 220052e:	fa768693          	addi	a3,a3,-89 # ffa7 <__text_end+0xbf9f>
 2200532:	8ff5                	and	a5,a5,a3
 2200534:	efc9                	bnez	a5,22005ce <psram_x8_32Mb_T200_init.constprop.0+0x356>
 2200536:	4785                	li	a5,1
 2200538:	02fca823          	sw	a5,48(s9)
 220053c:	020caa23          	sw	zero,52(s9)
 2200540:	020cac23          	sw	zero,56(s9)
 2200544:	058ca783          	lw	a5,88(s9)
 2200548:	efd1                	bnez	a5,22005e4 <psram_x8_32Mb_T200_init.constprop.0+0x36c>
 220054a:	060ca783          	lw	a5,96(s9)
 220054e:	e7dd                	bnez	a5,22005fc <psram_x8_32Mb_T200_init.constprop.0+0x384>
 2200550:	064ca783          	lw	a5,100(s9)
 2200554:	4501                	li	a0,0
 2200556:	efdd                	bnez	a5,2200614 <psram_x8_32Mb_T200_init.constprop.0+0x39c>
 2200558:	50f2                	lw	ra,60(sp)
 220055a:	5462                	lw	s0,56(sp)
 220055c:	54d2                	lw	s1,52(sp)
 220055e:	5942                	lw	s2,48(sp)
 2200560:	59b2                	lw	s3,44(sp)
 2200562:	5a22                	lw	s4,40(sp)
 2200564:	5a92                	lw	s5,36(sp)
 2200566:	5b02                	lw	s6,32(sp)
 2200568:	4bf2                	lw	s7,28(sp)
 220056a:	4c62                	lw	s8,24(sp)
 220056c:	4cd2                	lw	s9,20(sp)
 220056e:	4d42                	lw	s10,16(sp)
 2200570:	4db2                	lw	s11,12(sp)
 2200572:	6121                	addi	sp,sp,64
 2200574:	8082                	ret
 2200576:	02202537          	lui	a0,0x2202
 220057a:	08800593          	li	a1,136
 220057e:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200582:	fde02097          	auipc	ra,0xfde02
 2200586:	2a8080e7          	jalr	680(ra) # 282a <printf>
 220058a:	b5d5                	j	220046e <psram_x8_32Mb_T200_init.constprop.0+0x1f6>
 220058c:	02202537          	lui	a0,0x2202
 2200590:	10700593          	li	a1,263
 2200594:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200598:	fde02097          	auipc	ra,0xfde02
 220059c:	292080e7          	jalr	658(ra) # 282a <printf>
 22005a0:	bf59                	j	2200536 <psram_x8_32Mb_T200_init.constprop.0+0x2be>
 22005a2:	02202537          	lui	a0,0x2202
 22005a6:	0e400593          	li	a1,228
 22005aa:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22005ae:	fde02097          	auipc	ra,0xfde02
 22005b2:	27c080e7          	jalr	636(ra) # 282a <printf>
 22005b6:	b741                	j	2200536 <psram_x8_32Mb_T200_init.constprop.0+0x2be>
 22005b8:	02202537          	lui	a0,0x2202
 22005bc:	0a300593          	li	a1,163
 22005c0:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22005c4:	fde02097          	auipc	ra,0xfde02
 22005c8:	266080e7          	jalr	614(ra) # 282a <printf>
 22005cc:	b54d                	j	220046e <psram_x8_32Mb_T200_init.constprop.0+0x1f6>
 22005ce:	02202537          	lui	a0,0x2202
 22005d2:	11900593          	li	a1,281
 22005d6:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22005da:	fde02097          	auipc	ra,0xfde02
 22005de:	250080e7          	jalr	592(ra) # 282a <printf>
 22005e2:	bf91                	j	2200536 <psram_x8_32Mb_T200_init.constprop.0+0x2be>
 22005e4:	02202537          	lui	a0,0x2202
 22005e8:	21100593          	li	a1,529
 22005ec:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22005f0:	fde02097          	auipc	ra,0xfde02
 22005f4:	23a080e7          	jalr	570(ra) # 282a <printf>
 22005f8:	557d                	li	a0,-1
 22005fa:	bfb9                	j	2200558 <psram_x8_32Mb_T200_init.constprop.0+0x2e0>
 22005fc:	02202537          	lui	a0,0x2202
 2200600:	21300593          	li	a1,531
 2200604:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200608:	fde02097          	auipc	ra,0xfde02
 220060c:	222080e7          	jalr	546(ra) # 282a <printf>
 2200610:	557d                	li	a0,-1
 2200612:	b799                	j	2200558 <psram_x8_32Mb_T200_init.constprop.0+0x2e0>
 2200614:	02202537          	lui	a0,0x2202
 2200618:	21500593          	li	a1,533
 220061c:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200620:	fde02097          	auipc	ra,0xfde02
 2200624:	20a080e7          	jalr	522(ra) # 282a <printf>
 2200628:	557d                	li	a0,-1
 220062a:	b73d                	j	2200558 <psram_x8_32Mb_T200_init.constprop.0+0x2e0>
 220062c:	0000                	unimp
	...

02200630 <psram_x16_128Mb_T250_init.constprop.0>:
 2200630:	1101                	addi	sp,sp,-32
 2200632:	e060a7b7          	lui	a5,0xe060a
 2200636:	cc22                	sw	s0,24(sp)
 2200638:	ca26                	sw	s1,20(sp)
 220063a:	c84a                	sw	s2,16(sp)
 220063c:	ce06                	sw	ra,28(sp)
 220063e:	c64e                	sw	s3,12(sp)
 2200640:	02078793          	addi	a5,a5,32 # e060a020 <__end+0xde3cb668>
 2200644:	c15c                	sw	a5,4(a0)
 2200646:	07700793          	li	a5,119
 220064a:	cd5c                	sw	a5,28(a0)
 220064c:	022117b7          	lui	a5,0x2211
 2200650:	72178793          	addi	a5,a5,1825 # 2211721 <__hook_end+0xf9c9>
 2200654:	d11c                	sw	a5,32(a0)
 2200656:	02052223          	sw	zero,36(a0)
 220065a:	003607b7          	lui	a5,0x360
 220065e:	10052223          	sw	zero,260(a0)
 2200662:	43678793          	addi	a5,a5,1078 # 360436 <__text_end+0x35c42e>
 2200666:	10f52623          	sw	a5,268(a0)
 220066a:	6789                	lui	a5,0x2
 220066c:	02078793          	addi	a5,a5,32 # 2020 <hbp_lib_exec+0x274>
 2200670:	10f52823          	sw	a5,272(a0)
 2200674:	4785                	li	a5,1
 2200676:	10f52223          	sw	a5,260(a0)
 220067a:	10852783          	lw	a5,264(a0)
 220067e:	842a                	mv	s0,a0
 2200680:	10850913          	addi	s2,a0,264
 2200684:	8b85                	andi	a5,a5,1
 2200686:	44a9                	li	s1,10
 2200688:	ef91                	bnez	a5,22006a4 <psram_x16_128Mb_T250_init.constprop.0+0x74>
 220068a:	0001                	nop
 220068c:	4529                	li	a0,10
 220068e:	fde01097          	auipc	ra,0xfde01
 2200692:	4b8080e7          	jalr	1208(ra) # 1b46 <dly_lib_delay_us>
 2200696:	00092783          	lw	a5,0(s2)
 220069a:	14fd                	addi	s1,s1,-1
 220069c:	8b85                	andi	a5,a5,1
 220069e:	e399                	bnez	a5,22006a4 <psram_x16_128Mb_T250_init.constprop.0+0x74>
 22006a0:	fe0496e3          	bnez	s1,220068c <psram_x16_128Mb_T250_init.constprop.0+0x5c>
 22006a4:	c00407b7          	lui	a5,0xc0040
 22006a8:	17fd                	addi	a5,a5,-1
 22006aa:	14f42023          	sw	a5,320(s0)
 22006ae:	478d                	li	a5,3
 22006b0:	12f42623          	sw	a5,300(s0)
 22006b4:	4709                	li	a4,2
 22006b6:	12e42623          	sw	a4,300(s0)
 22006ba:	12e42623          	sw	a4,300(s0)
 22006be:	12f42623          	sw	a5,300(s0)
 22006c2:	09600513          	li	a0,150
 22006c6:	fde01097          	auipc	ra,0xfde01
 22006ca:	480080e7          	jalr	1152(ra) # 1b46 <dly_lib_delay_us>
 22006ce:	010007b7          	lui	a5,0x1000
 22006d2:	0785                	addi	a5,a5,1
 22006d4:	c41c                	sw	a5,8(s0)
 22006d6:	03000793          	li	a5,48
 22006da:	cc1c                	sw	a5,24(s0)
 22006dc:	03100793          	li	a5,49
 22006e0:	cc1c                	sw	a5,24(s0)
 22006e2:	4c1c                	lw	a5,24(s0)
 22006e4:	01840493          	addi	s1,s0,24
 22006e8:	4929                	li	s2,10
 22006ea:	1007f793          	andi	a5,a5,256
 22006ee:	1e079e63          	bnez	a5,22008ea <psram_x16_128Mb_T250_init.constprop.0+0x2ba>
 22006f2:	0001                	nop
 22006f4:	4505                	li	a0,1
 22006f6:	fde01097          	auipc	ra,0xfde01
 22006fa:	450080e7          	jalr	1104(ra) # 1b46 <dly_lib_delay_us>
 22006fe:	409c                	lw	a5,0(s1)
 2200700:	197d                	addi	s2,s2,-1
 2200702:	1007f793          	andi	a5,a5,256
 2200706:	1e079263          	bnez	a5,22008ea <psram_x16_128Mb_T250_init.constprop.0+0x2ba>
 220070a:	fe0915e3          	bnez	s2,22006f4 <psram_x16_128Mb_T250_init.constprop.0+0xc4>
 220070e:	6729                	lui	a4,0xa
 2200710:	010007b7          	lui	a5,0x1000
 2200714:	0785                	addi	a5,a5,1
 2200716:	c41c                	sw	a5,8(s0)
 2200718:	c818                	sw	a4,16(s0)
 220071a:	02000793          	li	a5,32
 220071e:	cc1c                	sw	a5,24(s0)
 2200720:	02100793          	li	a5,33
 2200724:	cc1c                	sw	a5,24(s0)
 2200726:	4c1c                	lw	a5,24(s0)
 2200728:	4929                	li	s2,10
 220072a:	1007f793          	andi	a5,a5,256
 220072e:	ef89                	bnez	a5,2200748 <psram_x16_128Mb_T250_init.constprop.0+0x118>
 2200730:	4505                	li	a0,1
 2200732:	fde01097          	auipc	ra,0xfde01
 2200736:	414080e7          	jalr	1044(ra) # 1b46 <dly_lib_delay_us>
 220073a:	409c                	lw	a5,0(s1)
 220073c:	197d                	addi	s2,s2,-1
 220073e:	1007f793          	andi	a5,a5,256
 2200742:	e399                	bnez	a5,2200748 <psram_x16_128Mb_T250_init.constprop.0+0x118>
 2200744:	fe0916e3          	bnez	s2,2200730 <psram_x16_128Mb_T250_init.constprop.0+0x100>
 2200748:	4529                	li	a0,10
 220074a:	fde01097          	auipc	ra,0xfde01
 220074e:	3fc080e7          	jalr	1020(ra) # 1b46 <dly_lib_delay_us>
 2200752:	03000793          	li	a5,48
 2200756:	00042423          	sw	zero,8(s0)
 220075a:	cc1c                	sw	a5,24(s0)
 220075c:	03100793          	li	a5,49
 2200760:	cc1c                	sw	a5,24(s0)
 2200762:	4c1c                	lw	a5,24(s0)
 2200764:	4929                	li	s2,10
 2200766:	1007f793          	andi	a5,a5,256
 220076a:	ef9d                	bnez	a5,22007a8 <psram_x16_128Mb_T250_init.constprop.0+0x178>
 220076c:	4505                	li	a0,1
 220076e:	fde01097          	auipc	ra,0xfde01
 2200772:	3d8080e7          	jalr	984(ra) # 1b46 <dly_lib_delay_us>
 2200776:	409c                	lw	a5,0(s1)
 2200778:	197d                	addi	s2,s2,-1
 220077a:	1007f793          	andi	a5,a5,256
 220077e:	e78d                	bnez	a5,22007a8 <psram_x16_128Mb_T250_init.constprop.0+0x178>
 2200780:	fe0916e3          	bnez	s2,220076c <psram_x16_128Mb_T250_init.constprop.0+0x13c>
 2200784:	02202537          	lui	a0,0x2202
 2200788:	19d00593          	li	a1,413
 220078c:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200790:	fde02097          	auipc	ra,0xfde02
 2200794:	09a080e7          	jalr	154(ra) # 282a <printf>
 2200798:	557d                	li	a0,-1
 220079a:	40f2                	lw	ra,28(sp)
 220079c:	4462                	lw	s0,24(sp)
 220079e:	44d2                	lw	s1,20(sp)
 22007a0:	4942                	lw	s2,16(sp)
 22007a2:	49b2                	lw	s3,12(sp)
 22007a4:	6105                	addi	sp,sp,32
 22007a6:	8082                	ret
 22007a8:	4858                	lw	a4,20(s0)
 22007aa:	6785                	lui	a5,0x1
 22007ac:	c8678793          	addi	a5,a5,-890 # c86 <loader_verify_img+0x4>
 22007b0:	3c07370b          	extu	a4,a4,15,0
 22007b4:	fcf718e3          	bne	a4,a5,2200784 <psram_x16_128Mb_T250_init.constprop.0+0x154>
 22007b8:	4785                	li	a5,1
 22007ba:	c41c                	sw	a5,8(s0)
 22007bc:	03000793          	li	a5,48
 22007c0:	cc1c                	sw	a5,24(s0)
 22007c2:	03100793          	li	a5,49
 22007c6:	cc1c                	sw	a5,24(s0)
 22007c8:	4c1c                	lw	a5,24(s0)
 22007ca:	4929                	li	s2,10
 22007cc:	1007f793          	andi	a5,a5,256
 22007d0:	e3a1                	bnez	a5,2200810 <psram_x16_128Mb_T250_init.constprop.0+0x1e0>
 22007d2:	0001                	nop
 22007d4:	4505                	li	a0,1
 22007d6:	fde01097          	auipc	ra,0xfde01
 22007da:	370080e7          	jalr	880(ra) # 1b46 <dly_lib_delay_us>
 22007de:	409c                	lw	a5,0(s1)
 22007e0:	197d                	addi	s2,s2,-1
 22007e2:	1007f793          	andi	a5,a5,256
 22007e6:	e78d                	bnez	a5,2200810 <psram_x16_128Mb_T250_init.constprop.0+0x1e0>
 22007e8:	fe0916e3          	bnez	s2,22007d4 <psram_x16_128Mb_T250_init.constprop.0+0x1a4>
 22007ec:	02202537          	lui	a0,0x2202
 22007f0:	1a200593          	li	a1,418
 22007f4:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22007f8:	fde02097          	auipc	ra,0xfde02
 22007fc:	032080e7          	jalr	50(ra) # 282a <printf>
 2200800:	40f2                	lw	ra,28(sp)
 2200802:	4462                	lw	s0,24(sp)
 2200804:	44d2                	lw	s1,20(sp)
 2200806:	4942                	lw	s2,16(sp)
 2200808:	49b2                	lw	s3,12(sp)
 220080a:	557d                	li	a0,-1
 220080c:	6105                	addi	sp,sp,32
 220080e:	8082                	ret
 2200810:	01442903          	lw	s2,20(s0)
 2200814:	47a9                	li	a5,10
 2200816:	3c09390b          	extu	s2,s2,15,0
 220081a:	fcf919e3          	bne	s2,a5,22007ec <psram_x16_128Mb_T250_init.constprop.0+0x1bc>
 220081e:	010007b7          	lui	a5,0x1000
 2200822:	c41c                	sw	a5,8(s0)
 2200824:	03000793          	li	a5,48
 2200828:	cc1c                	sw	a5,24(s0)
 220082a:	03100793          	li	a5,49
 220082e:	cc1c                	sw	a5,24(s0)
 2200830:	4c1c                	lw	a5,24(s0)
 2200832:	49a9                	li	s3,10
 2200834:	1007f793          	andi	a5,a5,256
 2200838:	e3a1                	bnez	a5,2200878 <psram_x16_128Mb_T250_init.constprop.0+0x248>
 220083a:	0001                	nop
 220083c:	4505                	li	a0,1
 220083e:	fde01097          	auipc	ra,0xfde01
 2200842:	308080e7          	jalr	776(ra) # 1b46 <dly_lib_delay_us>
 2200846:	409c                	lw	a5,0(s1)
 2200848:	19fd                	addi	s3,s3,-1
 220084a:	1007f793          	andi	a5,a5,256
 220084e:	e78d                	bnez	a5,2200878 <psram_x16_128Mb_T250_init.constprop.0+0x248>
 2200850:	fe0996e3          	bnez	s3,220083c <psram_x16_128Mb_T250_init.constprop.0+0x20c>
 2200854:	02202537          	lui	a0,0x2202
 2200858:	1a700593          	li	a1,423
 220085c:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200860:	fde02097          	auipc	ra,0xfde02
 2200864:	fca080e7          	jalr	-54(ra) # 282a <printf>
 2200868:	40f2                	lw	ra,28(sp)
 220086a:	4462                	lw	s0,24(sp)
 220086c:	44d2                	lw	s1,20(sp)
 220086e:	4942                	lw	s2,16(sp)
 2200870:	49b2                	lw	s3,12(sp)
 2200872:	557d                	li	a0,-1
 2200874:	6105                	addi	sp,sp,32
 2200876:	8082                	ret
 2200878:	4858                	lw	a4,20(s0)
 220087a:	67a5                	lui	a5,0x9
 220087c:	f2f78793          	addi	a5,a5,-209 # 8f2f <__text_end+0x4f27>
 2200880:	3c07370b          	extu	a4,a4,15,0
 2200884:	fcf718e3          	bne	a4,a5,2200854 <psram_x16_128Mb_T250_init.constprop.0+0x224>
 2200888:	010007b7          	lui	a5,0x1000
 220088c:	0785                	addi	a5,a5,1
 220088e:	c41c                	sw	a5,8(s0)
 2200890:	03000793          	li	a5,48
 2200894:	cc1c                	sw	a5,24(s0)
 2200896:	03100793          	li	a5,49
 220089a:	cc1c                	sw	a5,24(s0)
 220089c:	4c1c                	lw	a5,24(s0)
 220089e:	1007f793          	andi	a5,a5,256
 22008a2:	eb8d                	bnez	a5,22008d4 <psram_x16_128Mb_T250_init.constprop.0+0x2a4>
 22008a4:	4505                	li	a0,1
 22008a6:	fde01097          	auipc	ra,0xfde01
 22008aa:	2a0080e7          	jalr	672(ra) # 1b46 <dly_lib_delay_us>
 22008ae:	409c                	lw	a5,0(s1)
 22008b0:	197d                	addi	s2,s2,-1
 22008b2:	1007f793          	andi	a5,a5,256
 22008b6:	ef99                	bnez	a5,22008d4 <psram_x16_128Mb_T250_init.constprop.0+0x2a4>
 22008b8:	fe0916e3          	bnez	s2,22008a4 <psram_x16_128Mb_T250_init.constprop.0+0x274>
 22008bc:	02202537          	lui	a0,0x2202
 22008c0:	1ac00593          	li	a1,428
 22008c4:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 22008c8:	fde02097          	auipc	ra,0xfde02
 22008cc:	f62080e7          	jalr	-158(ra) # 282a <printf>
 22008d0:	557d                	li	a0,-1
 22008d2:	b5e1                	j	220079a <psram_x16_128Mb_T250_init.constprop.0+0x16a>
 22008d4:	4858                	lw	a4,20(s0)
 22008d6:	67c1                	lui	a5,0x10
 22008d8:	ffc78693          	addi	a3,a5,-4 # fffc <__text_end+0xbff4>
 22008dc:	8f75                	and	a4,a4,a3
 22008de:	fc078793          	addi	a5,a5,-64
 22008e2:	fcf71de3          	bne	a4,a5,22008bc <psram_x16_128Mb_T250_init.constprop.0+0x28c>
 22008e6:	4501                	li	a0,0
 22008e8:	bd4d                	j	220079a <psram_x16_128Mb_T250_init.constprop.0+0x16a>
 22008ea:	4858                	lw	a4,20(s0)
 22008ec:	6785                	lui	a5,0x1
 22008ee:	17fd                	addi	a5,a5,-1
 22008f0:	8f7d                	and	a4,a4,a5
 22008f2:	67a9                	lui	a5,0xa
 22008f4:	8f5d                	or	a4,a4,a5
 22008f6:	bd29                	j	2200710 <psram_x16_128Mb_T250_init.constprop.0+0xe0>
 22008f8:	0000                	unimp
	...

022008fc <psram_x16_256Mb_T250_init>:
 22008fc:	7179                	addi	sp,sp,-48
 22008fe:	e060a7b7          	lui	a5,0xe060a
 2200902:	d422                	sw	s0,40(sp)
 2200904:	d606                	sw	ra,44(sp)
 2200906:	d226                	sw	s1,36(sp)
 2200908:	d04a                	sw	s2,32(sp)
 220090a:	ce4e                	sw	s3,28(sp)
 220090c:	cc52                	sw	s4,24(sp)
 220090e:	ca56                	sw	s5,20(sp)
 2200910:	c85a                	sw	s6,16(sp)
 2200912:	c65e                	sw	s7,12(sp)
 2200914:	c462                	sw	s8,8(sp)
 2200916:	c266                	sw	s9,4(sp)
 2200918:	c06a                	sw	s10,0(sp)
 220091a:	02078793          	addi	a5,a5,32 # e060a020 <__end+0xde3cb668>
 220091e:	c15c                	sw	a5,4(a0)
 2200920:	000727b7          	lui	a5,0x72
 2200924:	73378793          	addi	a5,a5,1843 # 72733 <__text_end+0x6e72b>
 2200928:	d11c                	sw	a5,32(a0)
 220092a:	02052223          	sw	zero,36(a0)
 220092e:	20400793          	li	a5,516
 2200932:	10f52023          	sw	a5,256(a0)
 2200936:	6789                	lui	a5,0x2
 2200938:	02078793          	addi	a5,a5,32 # 2020 <hbp_lib_exec+0x274>
 220093c:	10f52823          	sw	a5,272(a0)
 2200940:	12f52023          	sw	a5,288(a0)
 2200944:	06300793          	li	a5,99
 2200948:	842a                	mv	s0,a0
 220094a:	22b7f663          	bgeu	a5,a1,2200b76 <psram_x16_256Mb_T250_init+0x27a>
 220094e:	00360a37          	lui	s4,0x360
 2200952:	428a0913          	addi	s2,s4,1064 # 360428 <__text_end+0x35c420>
 2200956:	10450993          	addi	s3,a0,260
 220095a:	10c50a93          	addi	s5,a0,268
 220095e:	11c50b13          	addi	s6,a0,284
 2200962:	11450c93          	addi	s9,a0,276
 2200966:	10850493          	addi	s1,a0,264
 220096a:	4c05                	li	s8,1
 220096c:	4bd1                	li	s7,20
 220096e:	4f0a0a13          	addi	s4,s4,1264
 2200972:	0009a023          	sw	zero,0(s3)
 2200976:	012aa023          	sw	s2,0(s5)
 220097a:	012b2023          	sw	s2,0(s6)
 220097e:	0189a023          	sw	s8,0(s3)
 2200982:	409c                	lw	a5,0(s1)
 2200984:	4d29                	li	s10,10
 2200986:	8b85                	andi	a5,a5,1
 2200988:	ef89                	bnez	a5,22009a2 <psram_x16_256Mb_T250_init+0xa6>
 220098a:	0001                	nop
 220098c:	4529                	li	a0,10
 220098e:	fde01097          	auipc	ra,0xfde01
 2200992:	1b8080e7          	jalr	440(ra) # 1b46 <dly_lib_delay_us>
 2200996:	409c                	lw	a5,0(s1)
 2200998:	1d7d                	addi	s10,s10,-1
 220099a:	8b85                	andi	a5,a5,1
 220099c:	e399                	bnez	a5,22009a2 <psram_x16_256Mb_T250_init+0xa6>
 220099e:	fe0d17e3          	bnez	s10,220098c <psram_x16_256Mb_T250_init+0x90>
 22009a2:	000ca783          	lw	a5,0(s9)
 22009a6:	3c87b78b          	extu	a5,a5,15,8
 22009aa:	00fbe663          	bltu	s7,a5,22009b6 <psram_x16_256Mb_T250_init+0xba>
 22009ae:	02890913          	addi	s2,s2,40
 22009b2:	fd4910e3          	bne	s2,s4,2200972 <psram_x16_256Mb_T250_init+0x76>
 22009b6:	040007b7          	lui	a5,0x4000
 22009ba:	70178793          	addi	a5,a5,1793 # 4000701 <__end+0x1dc1d49>
 22009be:	d41c                	sw	a5,40(s0)
 22009c0:	4785                	li	a5,1
 22009c2:	12f42623          	sw	a5,300(s0)
 22009c6:	12042623          	sw	zero,300(s0)
 22009ca:	12042623          	sw	zero,300(s0)
 22009ce:	12042623          	sw	zero,300(s0)
 22009d2:	12042623          	sw	zero,300(s0)
 22009d6:	12f42623          	sw	a5,300(s0)
 22009da:	09600513          	li	a0,150
 22009de:	fde01097          	auipc	ra,0xfde01
 22009e2:	168080e7          	jalr	360(ra) # 1b46 <dly_lib_delay_us>
 22009e6:	03000793          	li	a5,48
 22009ea:	00042423          	sw	zero,8(s0)
 22009ee:	cc1c                	sw	a5,24(s0)
 22009f0:	03100793          	li	a5,49
 22009f4:	cc1c                	sw	a5,24(s0)
 22009f6:	4c1c                	lw	a5,24(s0)
 22009f8:	01840493          	addi	s1,s0,24
 22009fc:	4929                	li	s2,10
 22009fe:	1007f793          	andi	a5,a5,256
 2200a02:	e7b1                	bnez	a5,2200a4e <psram_x16_256Mb_T250_init+0x152>
 2200a04:	4505                	li	a0,1
 2200a06:	fde01097          	auipc	ra,0xfde01
 2200a0a:	140080e7          	jalr	320(ra) # 1b46 <dly_lib_delay_us>
 2200a0e:	409c                	lw	a5,0(s1)
 2200a10:	197d                	addi	s2,s2,-1
 2200a12:	1007f793          	andi	a5,a5,256
 2200a16:	ef85                	bnez	a5,2200a4e <psram_x16_256Mb_T250_init+0x152>
 2200a18:	fe0916e3          	bnez	s2,2200a04 <psram_x16_256Mb_T250_init+0x108>
 2200a1c:	02202537          	lui	a0,0x2202
 2200a20:	16300593          	li	a1,355
 2200a24:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200a28:	fde02097          	auipc	ra,0xfde02
 2200a2c:	e02080e7          	jalr	-510(ra) # 282a <printf>
 2200a30:	557d                	li	a0,-1
 2200a32:	50b2                	lw	ra,44(sp)
 2200a34:	5422                	lw	s0,40(sp)
 2200a36:	5492                	lw	s1,36(sp)
 2200a38:	5902                	lw	s2,32(sp)
 2200a3a:	49f2                	lw	s3,28(sp)
 2200a3c:	4a62                	lw	s4,24(sp)
 2200a3e:	4ad2                	lw	s5,20(sp)
 2200a40:	4b42                	lw	s6,16(sp)
 2200a42:	4bb2                	lw	s7,12(sp)
 2200a44:	4c22                	lw	s8,8(sp)
 2200a46:	4c92                	lw	s9,4(sp)
 2200a48:	4d02                	lw	s10,0(sp)
 2200a4a:	6145                	addi	sp,sp,48
 2200a4c:	8082                	ret
 2200a4e:	4858                	lw	a4,20(s0)
 2200a50:	6785                	lui	a5,0x1
 2200a52:	e7678793          	addi	a5,a5,-394 # e76 <sfc_read_reg+0x6>
 2200a56:	3c07370b          	extu	a4,a4,15,0
 2200a5a:	fcf711e3          	bne	a4,a5,2200a1c <psram_x16_256Mb_T250_init+0x120>
 2200a5e:	4785                	li	a5,1
 2200a60:	c41c                	sw	a5,8(s0)
 2200a62:	03000793          	li	a5,48
 2200a66:	cc1c                	sw	a5,24(s0)
 2200a68:	03100793          	li	a5,49
 2200a6c:	cc1c                	sw	a5,24(s0)
 2200a6e:	4c1c                	lw	a5,24(s0)
 2200a70:	4929                	li	s2,10
 2200a72:	1007f793          	andi	a5,a5,256
 2200a76:	eb8d                	bnez	a5,2200aa8 <psram_x16_256Mb_T250_init+0x1ac>
 2200a78:	4505                	li	a0,1
 2200a7a:	fde01097          	auipc	ra,0xfde01
 2200a7e:	0cc080e7          	jalr	204(ra) # 1b46 <dly_lib_delay_us>
 2200a82:	409c                	lw	a5,0(s1)
 2200a84:	197d                	addi	s2,s2,-1
 2200a86:	1007f793          	andi	a5,a5,256
 2200a8a:	ef99                	bnez	a5,2200aa8 <psram_x16_256Mb_T250_init+0x1ac>
 2200a8c:	fe0916e3          	bnez	s2,2200a78 <psram_x16_256Mb_T250_init+0x17c>
 2200a90:	02202537          	lui	a0,0x2202
 2200a94:	16800593          	li	a1,360
 2200a98:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200a9c:	fde02097          	auipc	ra,0xfde02
 2200aa0:	d8e080e7          	jalr	-626(ra) # 282a <printf>
 2200aa4:	557d                	li	a0,-1
 2200aa6:	b771                	j	2200a32 <psram_x16_256Mb_T250_init+0x136>
 2200aa8:	485c                	lw	a5,20(s0)
 2200aaa:	4725                	li	a4,9
 2200aac:	3c07b78b          	extu	a5,a5,15,0
 2200ab0:	fee790e3          	bne	a5,a4,2200a90 <psram_x16_256Mb_T250_init+0x194>
 2200ab4:	010007b7          	lui	a5,0x1000
 2200ab8:	c41c                	sw	a5,8(s0)
 2200aba:	03000793          	li	a5,48
 2200abe:	cc1c                	sw	a5,24(s0)
 2200ac0:	03100793          	li	a5,49
 2200ac4:	cc1c                	sw	a5,24(s0)
 2200ac6:	4c1c                	lw	a5,24(s0)
 2200ac8:	4929                	li	s2,10
 2200aca:	1007f793          	andi	a5,a5,256
 2200ace:	eb8d                	bnez	a5,2200b00 <psram_x16_256Mb_T250_init+0x204>
 2200ad0:	4505                	li	a0,1
 2200ad2:	fde01097          	auipc	ra,0xfde01
 2200ad6:	074080e7          	jalr	116(ra) # 1b46 <dly_lib_delay_us>
 2200ada:	409c                	lw	a5,0(s1)
 2200adc:	197d                	addi	s2,s2,-1
 2200ade:	1007f793          	andi	a5,a5,256
 2200ae2:	ef99                	bnez	a5,2200b00 <psram_x16_256Mb_T250_init+0x204>
 2200ae4:	fe0916e3          	bnez	s2,2200ad0 <psram_x16_256Mb_T250_init+0x1d4>
 2200ae8:	02202537          	lui	a0,0x2202
 2200aec:	16d00593          	li	a1,365
 2200af0:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200af4:	fde02097          	auipc	ra,0xfde02
 2200af8:	d36080e7          	jalr	-714(ra) # 282a <printf>
 2200afc:	557d                	li	a0,-1
 2200afe:	bf15                	j	2200a32 <psram_x16_256Mb_T250_init+0x136>
 2200b00:	4858                	lw	a4,20(s0)
 2200b02:	67a5                	lui	a5,0x9
 2200b04:	f2f78793          	addi	a5,a5,-209 # 8f2f <__text_end+0x4f27>
 2200b08:	3c07370b          	extu	a4,a4,15,0
 2200b0c:	fcf71ee3          	bne	a4,a5,2200ae8 <psram_x16_256Mb_T250_init+0x1ec>
 2200b10:	010007b7          	lui	a5,0x1000
 2200b14:	0785                	addi	a5,a5,1
 2200b16:	c41c                	sw	a5,8(s0)
 2200b18:	03000793          	li	a5,48
 2200b1c:	cc1c                	sw	a5,24(s0)
 2200b1e:	03100793          	li	a5,49
 2200b22:	cc1c                	sw	a5,24(s0)
 2200b24:	4c1c                	lw	a5,24(s0)
 2200b26:	4929                	li	s2,10
 2200b28:	1007f793          	andi	a5,a5,256
 2200b2c:	eb95                	bnez	a5,2200b60 <psram_x16_256Mb_T250_init+0x264>
 2200b2e:	0001                	nop
 2200b30:	4505                	li	a0,1
 2200b32:	fde01097          	auipc	ra,0xfde01
 2200b36:	014080e7          	jalr	20(ra) # 1b46 <dly_lib_delay_us>
 2200b3a:	409c                	lw	a5,0(s1)
 2200b3c:	197d                	addi	s2,s2,-1
 2200b3e:	1007f793          	andi	a5,a5,256
 2200b42:	ef99                	bnez	a5,2200b60 <psram_x16_256Mb_T250_init+0x264>
 2200b44:	fe0916e3          	bnez	s2,2200b30 <psram_x16_256Mb_T250_init+0x234>
 2200b48:	02202537          	lui	a0,0x2202
 2200b4c:	17700593          	li	a1,375
 2200b50:	ba850513          	addi	a0,a0,-1112 # 2201ba8 <__cmdl_end+0x24>
 2200b54:	fde02097          	auipc	ra,0xfde02
 2200b58:	cd6080e7          	jalr	-810(ra) # 282a <printf>
 2200b5c:	557d                	li	a0,-1
 2200b5e:	bdd1                	j	2200a32 <psram_x16_256Mb_T250_init+0x136>
 2200b60:	4858                	lw	a4,20(s0)
 2200b62:	67c1                	lui	a5,0x10
 2200b64:	ffc78693          	addi	a3,a5,-4 # fffc <__text_end+0xbff4>
 2200b68:	8f75                	and	a4,a4,a3
 2200b6a:	fc078793          	addi	a5,a5,-64
 2200b6e:	fcf71de3          	bne	a4,a5,2200b48 <psram_x16_256Mb_T250_init+0x24c>
 2200b72:	4501                	li	a0,0
 2200b74:	bd7d                	j	2200a32 <psram_x16_256Mb_T250_init+0x136>
 2200b76:	00b607b7          	lui	a5,0xb60
 2200b7a:	10052223          	sw	zero,260(a0)
 2200b7e:	42878793          	addi	a5,a5,1064 # b60428 <__text_end+0xb5c420>
 2200b82:	10f52623          	sw	a5,268(a0)
 2200b86:	10f52e23          	sw	a5,284(a0)
 2200b8a:	4785                	li	a5,1
 2200b8c:	10f52223          	sw	a5,260(a0)
 2200b90:	040007b7          	lui	a5,0x4000
 2200b94:	70078793          	addi	a5,a5,1792 # 4000700 <__end+0x1dc1d48>
 2200b98:	d51c                	sw	a5,40(a0)
 2200b9a:	b51d                	j	22009c0 <psram_x16_256Mb_T250_init+0xc4>
	...

02200b9e <setup_seq>:
 2200b9e:	4785                	li	a5,1
 2200ba0:	d91c                	sw	a5,48(a0)
 2200ba2:	d94c                	sw	a1,52(a0)
 2200ba4:	02052c23          	sw	zero,56(a0)
 2200ba8:	8082                	ret

02200baa <hook_ddr_init>:
 2200baa:	1101                	addi	sp,sp,-32
 2200bac:	cc22                	sw	s0,24(sp)
 2200bae:	ca26                	sw	s1,20(sp)
 2200bb0:	c64e                	sw	s3,12(sp)
 2200bb2:	ce06                	sw	ra,28(sp)
 2200bb4:	c84a                	sw	s2,16(sp)
 2200bb6:	84ae                	mv	s1,a1
 2200bb8:	8432                	mv	s0,a2
 2200bba:	89b6                	mv	s3,a3
 2200bbc:	e121                	bnez	a0,2200bfc <hook_ddr_init+0x52>
 2200bbe:	02059363          	bnez	a1,2200be4 <hook_ddr_init+0x3a>
 2200bc2:	022025b7          	lui	a1,0x2202
 2200bc6:	4519                	li	a0,6
 2200bc8:	bc058593          	addi	a1,a1,-1088 # 2201bc0 <__cmdl_end+0x3c>
 2200bcc:	fde02097          	auipc	ra,0xfde02
 2200bd0:	cd2080e7          	jalr	-814(ra) # 289e <ts_log>
 2200bd4:	40f2                	lw	ra,28(sp)
 2200bd6:	4462                	lw	s0,24(sp)
 2200bd8:	44d2                	lw	s1,20(sp)
 2200bda:	49b2                	lw	s3,12(sp)
 2200bdc:	854a                	mv	a0,s2
 2200bde:	4942                	lw	s2,16(sp)
 2200be0:	6105                	addi	sp,sp,32
 2200be2:	8082                	ret
 2200be4:	4505                	li	a0,1
 2200be6:	c7cff0ef          	jal	ra,2200062 <soc_get_ddr_bank_regbase>
 2200bea:	4785                	li	a5,1
 2200bec:	04f40763          	beq	s0,a5,2200c3a <hook_ddr_init+0x90>
 2200bf0:	02041b63          	bnez	s0,2200c26 <hook_ddr_init+0x7c>
 2200bf4:	e84ff0ef          	jal	ra,2200278 <psram_x8_32Mb_T200_init.constprop.0>
 2200bf8:	892a                	mv	s2,a0
 2200bfa:	b7e1                	j	2200bc2 <hook_ddr_init+0x18>
 2200bfc:	4501                	li	a0,0
 2200bfe:	c64ff0ef          	jal	ra,2200062 <soc_get_ddr_bank_regbase>
 2200c02:	4785                	li	a5,1
 2200c04:	02f40563          	beq	s0,a5,2200c2e <hook_ddr_init+0x84>
 2200c08:	00041b63          	bnez	s0,2200c1e <hook_ddr_init+0x74>
 2200c0c:	e6cff0ef          	jal	ra,2200278 <psram_x8_32Mb_T200_init.constprop.0>
 2200c10:	892a                	mv	s2,a0
 2200c12:	d8c5                	beqz	s1,2200bc2 <hook_ddr_init+0x18>
 2200c14:	4505                	li	a0,1
 2200c16:	c4cff0ef          	jal	ra,2200062 <soc_get_ddr_bank_regbase>
 2200c1a:	e411                	bnez	s0,2200c26 <hook_ddr_init+0x7c>
 2200c1c:	bfe1                	j	2200bf4 <hook_ddr_init+0x4a>
 2200c1e:	85ce                	mv	a1,s3
 2200c20:	39f1                	jal	22008fc <psram_x16_256Mb_T250_init>
 2200c22:	892a                	mv	s2,a0
 2200c24:	b7fd                	j	2200c12 <hook_ddr_init+0x68>
 2200c26:	85ce                	mv	a1,s3
 2200c28:	39d1                	jal	22008fc <psram_x16_256Mb_T250_init>
 2200c2a:	892a                	mv	s2,a0
 2200c2c:	bf59                	j	2200bc2 <hook_ddr_init+0x18>
 2200c2e:	3409                	jal	2200630 <psram_x16_128Mb_T250_init.constprop.0>
 2200c30:	892a                	mv	s2,a0
 2200c32:	d8c1                	beqz	s1,2200bc2 <hook_ddr_init+0x18>
 2200c34:	4505                	li	a0,1
 2200c36:	c2cff0ef          	jal	ra,2200062 <soc_get_ddr_bank_regbase>
 2200c3a:	3add                	jal	2200630 <psram_x16_128Mb_T250_init.constprop.0>
 2200c3c:	892a                	mv	s2,a0
 2200c3e:	b751                	j	2200bc2 <hook_ddr_init+0x18>

02200c40 <efuse_hal_write_bit>:
 2200c40:	557d                	li	a0,-1
 2200c42:	8082                	ret

02200c44 <efuse_hal_writel>:
 2200c44:	557d                	li	a0,-1
 2200c46:	8082                	ret

02200c48 <efuse_read_pubkey>:
 2200c48:	8082                	ret

02200c4a <efuse_init_pubkey>:
 2200c4a:	8082                	ret

02200c4c <efuse_reinit>:
 2200c4c:	8082                	ret

02200c4e <hook_uart_read>:
 2200c4e:	7179                	addi	sp,sp,-48
 2200c50:	d04a                	sw	s2,32(sp)
 2200c52:	cc52                	sw	s4,24(sp)
 2200c54:	ca56                	sw	s5,20(sp)
 2200c56:	d606                	sw	ra,44(sp)
 2200c58:	d422                	sw	s0,40(sp)
 2200c5a:	d226                	sw	s1,36(sp)
 2200c5c:	ce4e                	sw	s3,28(sp)
 2200c5e:	c85a                	sw	s6,16(sp)
 2200c60:	892e                	mv	s2,a1
 2200c62:	8a32                	mv	s4,a2
 2200c64:	8ab6                	mv	s5,a3
 2200c66:	c1eff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200c6a:	cd31                	beqz	a0,2200cc6 <hook_uart_read+0x78>
 2200c6c:	89aa                	mv	s3,a0
 2200c6e:	01450413          	addi	s0,a0,20
 2200c72:	01490b33          	add	s6,s2,s4
 2200c76:	4485                	li	s1,1
 2200c78:	020a0963          	beqz	s4,2200caa <hook_uart_read+0x5c>
 2200c7c:	85d6                	mv	a1,s5
 2200c7e:	0028                	addi	a0,sp,8
 2200c80:	fde01097          	auipc	ra,0xfde01
 2200c84:	e7c080e7          	jalr	-388(ra) # 1afc <dly_lib_init_ctx>
 2200c88:	a039                	j	2200c96 <hook_uart_read+0x48>
 2200c8a:	fde01097          	auipc	ra,0xfde01
 2200c8e:	e96080e7          	jalr	-362(ra) # 1b20 <dly_lib_timeout>
 2200c92:	00950e63          	beq	a0,s1,2200cae <hook_uart_read+0x60>
 2200c96:	401c                	lw	a5,0(s0)
 2200c98:	0028                	addi	a0,sp,8
 2200c9a:	8b85                	andi	a5,a5,1
 2200c9c:	d7fd                	beqz	a5,2200c8a <hook_uart_read+0x3c>
 2200c9e:	0009a783          	lw	a5,0(s3)
 2200ca2:	1819578b          	sbia	a5,(s2),1,0
 2200ca6:	fd2b1be3          	bne	s6,s2,2200c7c <hook_uart_read+0x2e>
 2200caa:	4501                	li	a0,0
 2200cac:	a019                	j	2200cb2 <hook_uart_read+0x64>
 2200cae:	07400513          	li	a0,116
 2200cb2:	50b2                	lw	ra,44(sp)
 2200cb4:	5422                	lw	s0,40(sp)
 2200cb6:	5492                	lw	s1,36(sp)
 2200cb8:	5902                	lw	s2,32(sp)
 2200cba:	49f2                	lw	s3,28(sp)
 2200cbc:	4a62                	lw	s4,24(sp)
 2200cbe:	4ad2                	lw	s5,20(sp)
 2200cc0:	4b42                	lw	s6,16(sp)
 2200cc2:	6145                	addi	sp,sp,48
 2200cc4:	8082                	ret
 2200cc6:	f7a00513          	li	a0,-134
 2200cca:	b7e5                	j	2200cb2 <hook_uart_read+0x64>

02200ccc <hook_uart_write>:
 2200ccc:	7179                	addi	sp,sp,-48
 2200cce:	d04a                	sw	s2,32(sp)
 2200cd0:	ce4e                	sw	s3,28(sp)
 2200cd2:	ca56                	sw	s5,20(sp)
 2200cd4:	d606                	sw	ra,44(sp)
 2200cd6:	d422                	sw	s0,40(sp)
 2200cd8:	d226                	sw	s1,36(sp)
 2200cda:	cc52                	sw	s4,24(sp)
 2200cdc:	c85a                	sw	s6,16(sp)
 2200cde:	892e                	mv	s2,a1
 2200ce0:	89b2                	mv	s3,a2
 2200ce2:	8ab6                	mv	s5,a3
 2200ce4:	ba0ff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200ce8:	cd31                	beqz	a0,2200d44 <hook_uart_write+0x78>
 2200cea:	02098f63          	beqz	s3,2200d28 <hook_uart_write+0x5c>
 2200cee:	8a2a                	mv	s4,a0
 2200cf0:	07c50413          	addi	s0,a0,124
 2200cf4:	99ca                	add	s3,s3,s2
 2200cf6:	4485                	li	s1,1
 2200cf8:	85d6                	mv	a1,s5
 2200cfa:	0028                	addi	a0,sp,8
 2200cfc:	fde01097          	auipc	ra,0xfde01
 2200d00:	e00080e7          	jalr	-512(ra) # 1afc <dly_lib_init_ctx>
 2200d04:	00094b03          	lbu	s6,0(s2)
 2200d08:	a039                	j	2200d16 <hook_uart_write+0x4a>
 2200d0a:	fde01097          	auipc	ra,0xfde01
 2200d0e:	e16080e7          	jalr	-490(ra) # 1b20 <dly_lib_timeout>
 2200d12:	00950d63          	beq	a0,s1,2200d2c <hook_uart_write+0x60>
 2200d16:	401c                	lw	a5,0(s0)
 2200d18:	0028                	addi	a0,sp,8
 2200d1a:	8b89                	andi	a5,a5,2
 2200d1c:	d7fd                	beqz	a5,2200d0a <hook_uart_write+0x3e>
 2200d1e:	016a2023          	sw	s6,0(s4)
 2200d22:	0905                	addi	s2,s2,1
 2200d24:	fd391ae3          	bne	s2,s3,2200cf8 <hook_uart_write+0x2c>
 2200d28:	4501                	li	a0,0
 2200d2a:	a019                	j	2200d30 <hook_uart_write+0x64>
 2200d2c:	f8c00513          	li	a0,-116
 2200d30:	50b2                	lw	ra,44(sp)
 2200d32:	5422                	lw	s0,40(sp)
 2200d34:	5492                	lw	s1,36(sp)
 2200d36:	5902                	lw	s2,32(sp)
 2200d38:	49f2                	lw	s3,28(sp)
 2200d3a:	4a62                	lw	s4,24(sp)
 2200d3c:	4ad2                	lw	s5,20(sp)
 2200d3e:	4b42                	lw	s6,16(sp)
 2200d40:	6145                	addi	sp,sp,48
 2200d42:	8082                	ret
 2200d44:	f7a00513          	li	a0,-134
 2200d48:	b7e5                	j	2200d30 <hook_uart_write+0x64>

02200d4a <hook_uart_putc>:
 2200d4a:	1141                	addi	sp,sp,-16
 2200d4c:	c422                	sw	s0,8(sp)
 2200d4e:	c606                	sw	ra,12(sp)
 2200d50:	842e                	mv	s0,a1
 2200d52:	b32ff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200d56:	cd01                	beqz	a0,2200d6e <hook_uart_putc+0x24>
 2200d58:	07c50693          	addi	a3,a0,124
 2200d5c:	00100737          	lui	a4,0x100
 2200d60:	a011                	j	2200d64 <hook_uart_putc+0x1a>
 2200d62:	c711                	beqz	a4,2200d6e <hook_uart_putc+0x24>
 2200d64:	429c                	lw	a5,0(a3)
 2200d66:	177d                	addi	a4,a4,-1
 2200d68:	8b89                	andi	a5,a5,2
 2200d6a:	dfe5                	beqz	a5,2200d62 <hook_uart_putc+0x18>
 2200d6c:	c100                	sw	s0,0(a0)
 2200d6e:	40b2                	lw	ra,12(sp)
 2200d70:	4422                	lw	s0,8(sp)
 2200d72:	0141                	addi	sp,sp,16
 2200d74:	8082                	ret
	...

02200d78 <hook_uart_puts>:
 2200d78:	1101                	addi	sp,sp,-32
 2200d7a:	ca26                	sw	s1,20(sp)
 2200d7c:	c84a                	sw	s2,16(sp)
 2200d7e:	c64e                	sw	s3,12(sp)
 2200d80:	c452                	sw	s4,8(sp)
 2200d82:	c256                	sw	s5,4(sp)
 2200d84:	ce06                	sw	ra,28(sp)
 2200d86:	cc22                	sw	s0,24(sp)
 2200d88:	8aae                	mv	s5,a1
 2200d8a:	84aa                	mv	s1,a0
 2200d8c:	06558993          	addi	s3,a1,101
 2200d90:	4929                	li	s2,10
 2200d92:	4a35                	li	s4,13
 2200d94:	981ac40b          	lbuia	s0,(s5),1,0
 2200d98:	c405                	beqz	s0,2200dc0 <hook_uart_puts+0x48>
 2200d9a:	03240c63          	beq	s0,s2,2200dd2 <hook_uart_puts+0x5a>
 2200d9e:	8526                	mv	a0,s1
 2200da0:	ae4ff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200da4:	cd01                	beqz	a0,2200dbc <hook_uart_puts+0x44>
 2200da6:	07c50693          	addi	a3,a0,124
 2200daa:	00100737          	lui	a4,0x100
 2200dae:	a011                	j	2200db2 <hook_uart_puts+0x3a>
 2200db0:	c711                	beqz	a4,2200dbc <hook_uart_puts+0x44>
 2200db2:	429c                	lw	a5,0(a3)
 2200db4:	177d                	addi	a4,a4,-1
 2200db6:	8b89                	andi	a5,a5,2
 2200db8:	dfe5                	beqz	a5,2200db0 <hook_uart_puts+0x38>
 2200dba:	c100                	sw	s0,0(a0)
 2200dbc:	fd3a9ce3          	bne	s5,s3,2200d94 <hook_uart_puts+0x1c>
 2200dc0:	40f2                	lw	ra,28(sp)
 2200dc2:	4462                	lw	s0,24(sp)
 2200dc4:	44d2                	lw	s1,20(sp)
 2200dc6:	4942                	lw	s2,16(sp)
 2200dc8:	49b2                	lw	s3,12(sp)
 2200dca:	4a22                	lw	s4,8(sp)
 2200dcc:	4a92                	lw	s5,4(sp)
 2200dce:	6105                	addi	sp,sp,32
 2200dd0:	8082                	ret
 2200dd2:	8526                	mv	a0,s1
 2200dd4:	ab0ff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200dd8:	d179                	beqz	a0,2200d9e <hook_uart_puts+0x26>
 2200dda:	07c50693          	addi	a3,a0,124
 2200dde:	00100737          	lui	a4,0x100
 2200de2:	a011                	j	2200de6 <hook_uart_puts+0x6e>
 2200de4:	df4d                	beqz	a4,2200d9e <hook_uart_puts+0x26>
 2200de6:	429c                	lw	a5,0(a3)
 2200de8:	177d                	addi	a4,a4,-1
 2200dea:	8b89                	andi	a5,a5,2
 2200dec:	dfe5                	beqz	a5,2200de4 <hook_uart_puts+0x6c>
 2200dee:	01452023          	sw	s4,0(a0)
 2200df2:	b775                	j	2200d9e <hook_uart_puts+0x26>
	...

02200df6 <hook_uart_init>:
 2200df6:	1141                	addi	sp,sp,-16
 2200df8:	c226                	sw	s1,4(sp)
 2200dfa:	c04a                	sw	s2,0(sp)
 2200dfc:	c606                	sw	ra,12(sp)
 2200dfe:	c422                	sw	s0,8(sp)
 2200e00:	84aa                	mv	s1,a0
 2200e02:	892e                	mv	s2,a1
 2200e04:	a80ff0ef          	jal	ra,2200084 <soc_get_uart_regbase>
 2200e08:	cd31                	beqz	a0,2200e64 <hook_uart_init+0x6e>
 2200e0a:	842a                	mv	s0,a0
 2200e0c:	a6eff0ef          	jal	ra,220007a <soc_get_osc_clk>
 2200e10:	0506                	slli	a0,a0,0x1
 2200e12:	0912                	slli	s2,s2,0x4
 2200e14:	03255533          	divu	a0,a0,s2
 2200e18:	6741                	lui	a4,0x10
 2200e1a:	1779                	addi	a4,a4,-2
 2200e1c:	00157793          	andi	a5,a0,1
 2200e20:	8105                	srli	a0,a0,0x1
 2200e22:	953e                	add	a0,a0,a5
 2200e24:	fff50793          	addi	a5,a0,-1
 2200e28:	02f76063          	bltu	a4,a5,2200e48 <hook_uart_init+0x52>
 2200e2c:	445c                	lw	a5,12(s0)
 2200e2e:	0ff57713          	zext.b	a4,a0
 2200e32:	8121                	srli	a0,a0,0x8
 2200e34:	0837e693          	ori	a3,a5,131
 2200e38:	c454                	sw	a3,12(s0)
 2200e3a:	c018                	sw	a4,0(s0)
 2200e3c:	f7f7f793          	andi	a5,a5,-129
 2200e40:	c048                	sw	a0,4(s0)
 2200e42:	0037e793          	ori	a5,a5,3
 2200e46:	c45c                	sw	a5,12(s0)
 2200e48:	479d                	li	a5,7
 2200e4a:	c41c                	sw	a5,8(s0)
 2200e4c:	8526                	mv	a0,s1
 2200e4e:	00042223          	sw	zero,4(s0)
 2200e52:	a4aff0ef          	jal	ra,220009c <soc_uart_set_mux>
 2200e56:	4501                	li	a0,0
 2200e58:	40b2                	lw	ra,12(sp)
 2200e5a:	4422                	lw	s0,8(sp)
 2200e5c:	4492                	lw	s1,4(sp)
 2200e5e:	4902                	lw	s2,0(sp)
 2200e60:	0141                	addi	sp,sp,16
 2200e62:	8082                	ret
 2200e64:	f7a00513          	li	a0,-134
 2200e68:	bfc5                	j	2200e58 <hook_uart_init+0x62>
	...

02200e6c <cmdl_reboot>:
 2200e6c:	1141                	addi	sp,sp,-16
 2200e6e:	c226                	sw	s1,4(sp)
 2200e70:	c606                	sw	ra,12(sp)
 2200e72:	c422                	sw	s0,8(sp)
 2200e74:	4789                	li	a5,2
 2200e76:	7d000493          	li	s1,2000
 2200e7a:	02f50363          	beq	a0,a5,2200ea0 <cmdl_reboot+0x34>
 2200e7e:	6b2000e7          	jalr	1714(zero) # 0 <__DYNAMIC>
 2200e82:	4401                	li	s0,0
 2200e84:	0405                	addi	s0,s0,1
 2200e86:	3e800513          	li	a0,1000
 2200e8a:	fde01097          	auipc	ra,0xfde01
 2200e8e:	cbc080e7          	jalr	-836(ra) # 1b46 <dly_lib_delay_us>
 2200e92:	fe8499e3          	bne	s1,s0,2200e84 <cmdl_reboot+0x18>
 2200e96:	1f400513          	li	a0,500
 2200e9a:	68c000e7          	jalr	1676(zero) # 0 <__DYNAMIC>
 2200e9e:	a001                	j	2200e9e <cmdl_reboot+0x32>
 2200ea0:	41c8                	lw	a0,4(a1)
 2200ea2:	fde01097          	auipc	ra,0xfde01
 2200ea6:	d8e080e7          	jalr	-626(ra) # 1c30 <ts_strtoul>
 2200eaa:	47a9                	li	a5,10
 2200eac:	00a7ec63          	bltu	a5,a0,2200ec4 <cmdl_reboot+0x58>
 2200eb0:	4789                	li	a5,2
 2200eb2:	00f56763          	bltu	a0,a5,2200ec0 <cmdl_reboot+0x54>
 2200eb6:	3e800493          	li	s1,1000
 2200eba:	029504b3          	mul	s1,a0,s1
 2200ebe:	b7c1                	j	2200e7e <cmdl_reboot+0x12>
 2200ec0:	4509                	li	a0,2
 2200ec2:	bfd5                	j	2200eb6 <cmdl_reboot+0x4a>
 2200ec4:	4529                	li	a0,10
 2200ec6:	b7ed                	j	2200eb0 <cmdl_reboot+0x44>
	...

02200eca <hook_snand_get_status>:
 2200eca:	1101                	addi	sp,sp,-32
 2200ecc:	77f1                	lui	a5,0xffffc
 2200ece:	cc22                	sw	s0,24(sp)
 2200ed0:	07bd                	addi	a5,a5,15
 2200ed2:	0007a437          	lui	s0,0x7a
 2200ed6:	ca26                	sw	s1,20(sp)
 2200ed8:	ce06                	sw	ra,28(sp)
 2200eda:	84aa                	mv	s1,a0
 2200edc:	00f11023          	sh	a5,0(sp)
 2200ee0:	00010223          	sb	zero,4(sp)
 2200ee4:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 2200ee8:	a801                	j	2200ef8 <hook_snand_get_status+0x2e>
 2200eea:	00414783          	lbu	a5,4(sp)
 2200eee:	0017f713          	andi	a4,a5,1
 2200ef2:	c31d                	beqz	a4,2200f18 <hook_snand_get_status+0x4e>
 2200ef4:	0a040563          	beqz	s0,2200f9e <hook_snand_get_status+0xd4>
 2200ef8:	4685                	li	a3,1
 2200efa:	0050                	addi	a2,sp,4
 2200efc:	4589                	li	a1,2
 2200efe:	850a                	mv	a0,sp
 2200f00:	fde00097          	auipc	ra,0xfde00
 2200f04:	f70080e7          	jalr	-144(ra) # e70 <sfc_read_reg>
 2200f08:	147d                	addi	s0,s0,-1
 2200f0a:	d165                	beqz	a0,2200eea <hook_snand_get_status+0x20>
 2200f0c:	557d                	li	a0,-1
 2200f0e:	40f2                	lw	ra,28(sp)
 2200f10:	4462                	lw	s0,24(sp)
 2200f12:	44d2                	lw	s1,20(sp)
 2200f14:	6105                	addi	sp,sp,32
 2200f16:	8082                	ret
 2200f18:	4709                	li	a4,2
 2200f1a:	02e48163          	beq	s1,a4,2200f3c <hook_snand_get_status+0x72>
 2200f1e:	4705                	li	a4,1
 2200f20:	fee497e3          	bne	s1,a4,2200f0e <hook_snand_get_status+0x44>
 2200f24:	40f2                	lw	ra,28(sp)
 2200f26:	4462                	lw	s0,24(sp)
 2200f28:	a5fbb737          	lui	a4,0xa5fbb
 2200f2c:	8bb1                	andi	a5,a5,12
 2200f2e:	dfe70713          	addi	a4,a4,-514 # a5fbadfe <__end+0xa3d7c446>
 2200f32:	44d2                	lw	s1,20(sp)
 2200f34:	42f7150b          	mvnez	a0,a4,a5
 2200f38:	6105                	addi	sp,sp,32
 2200f3a:	8082                	ret
 2200f3c:	0307f793          	andi	a5,a5,48
 2200f40:	d7f9                	beqz	a5,2200f0e <hook_snand_get_status+0x44>
 2200f42:	02000713          	li	a4,32
 2200f46:	06e78863          	beq	a5,a4,2200fb6 <hook_snand_get_status+0xec>
 2200f4a:	03000713          	li	a4,48
 2200f4e:	04e78f63          	beq	a5,a4,2200fac <hook_snand_get_status+0xe2>
 2200f52:	77fd                	lui	a5,0xfffff
 2200f54:	07bd                	addi	a5,a5,15
 2200f56:	4685                	li	a3,1
 2200f58:	0070                	addi	a2,sp,12
 2200f5a:	4589                	li	a1,2
 2200f5c:	0028                	addi	a0,sp,8
 2200f5e:	00f11423          	sh	a5,8(sp)
 2200f62:	00010623          	sb	zero,12(sp)
 2200f66:	fde00097          	auipc	ra,0xfde00
 2200f6a:	f0a080e7          	jalr	-246(ra) # e70 <sfc_read_reg>
 2200f6e:	fd59                	bnez	a0,2200f0c <hook_snand_get_status+0x42>
 2200f70:	022027b7          	lui	a5,0x2202
 2200f74:	ce07a783          	lw	a5,-800(a5) # 2201ce0 <sst_ctx>
 2200f78:	470d                	li	a4,3
 2200f7a:	17f1                	addi	a5,a5,-4
 2200f7c:	02f76863          	bltu	a4,a5,2200fac <hook_snand_get_status+0xe2>
 2200f80:	00c14683          	lbu	a3,12(sp)
 2200f84:	0ff7f793          	zext.b	a5,a5
 2200f88:	a5fbb737          	lui	a4,0xa5fbb
 2200f8c:	1446b68b          	extu	a3,a3,5,4
 2200f90:	00f6b7b3          	sltu	a5,a3,a5
 2200f94:	dff70713          	addi	a4,a4,-513 # a5fbadff <__end+0xa3d7c447>
 2200f98:	40f7150b          	mveqz	a0,a4,a5
 2200f9c:	bf8d                	j	2200f0e <hook_snand_get_status+0x44>
 2200f9e:	40f2                	lw	ra,28(sp)
 2200fa0:	4462                	lw	s0,24(sp)
 2200fa2:	44d2                	lw	s1,20(sp)
 2200fa4:	f8c00513          	li	a0,-116
 2200fa8:	6105                	addi	sp,sp,32
 2200faa:	8082                	ret
 2200fac:	a5fbb537          	lui	a0,0xa5fbb
 2200fb0:	dff50513          	addi	a0,a0,-513 # a5fbadff <__end+0xa3d7c447>
 2200fb4:	bfa9                	j	2200f0e <hook_snand_get_status+0x44>
 2200fb6:	a5fbb537          	lui	a0,0xa5fbb
 2200fba:	e0050513          	addi	a0,a0,-512 # a5fbae00 <__end+0xa3d7c448>
 2200fbe:	bf81                	j	2200f0e <hook_snand_get_status+0x44>

02200fc0 <hook_snand_mark_bad>:
 2200fc0:	080007b7          	lui	a5,0x8000
 2200fc4:	0ef57d63          	bgeu	a0,a5,22010be <hook_snand_mark_bad+0xfe>
 2200fc8:	7179                	addi	sp,sp,-48
 2200fca:	77f1                	lui	a5,0xffffc
 2200fcc:	d422                	sw	s0,40(sp)
 2200fce:	d226                	sw	s1,36(sp)
 2200fd0:	07bd                	addi	a5,a5,15
 2200fd2:	01155493          	srli	s1,a0,0x11
 2200fd6:	0007a437          	lui	s0,0x7a
 2200fda:	d606                	sw	ra,44(sp)
 2200fdc:	049a                	slli	s1,s1,0x6
 2200fde:	00f11a23          	sh	a5,20(sp)
 2200fe2:	00010c23          	sb	zero,24(sp)
 2200fe6:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 2200fea:	a039                	j	2200ff8 <hook_snand_mark_bad+0x38>
 2200fec:	01814783          	lbu	a5,24(sp)
 2200ff0:	8b85                	andi	a5,a5,1
 2200ff2:	cf99                	beqz	a5,2201010 <hook_snand_mark_bad+0x50>
 2200ff4:	00040c63          	beqz	s0,220100c <hook_snand_mark_bad+0x4c>
 2200ff8:	4685                	li	a3,1
 2200ffa:	0830                	addi	a2,sp,24
 2200ffc:	4589                	li	a1,2
 2200ffe:	0848                	addi	a0,sp,20
 2201000:	fde00097          	auipc	ra,0xfde00
 2201004:	e70080e7          	jalr	-400(ra) # e70 <sfc_read_reg>
 2201008:	147d                	addi	s0,s0,-1
 220100a:	d16d                	beqz	a0,2200fec <hook_snand_mark_bad+0x2c>
 220100c:	5541                	li	a0,-16
 220100e:	a041                	j	220108e <hook_snand_mark_bad+0xce>
 2201010:	cc02                	sw	zero,24(sp)
 2201012:	fde00097          	auipc	ra,0xfde00
 2201016:	0ee080e7          	jalr	238(ra) # 1100 <lib_sfc_write_en>
 220101a:	6785                	lui	a5,0x1
 220101c:	80278793          	addi	a5,a5,-2046 # 802 <ssbus_drv_write+0x28>
 2201020:	4689                	li	a3,2
 2201022:	0830                	addi	a2,sp,24
 2201024:	458d                	li	a1,3
 2201026:	0068                	addi	a0,sp,12
 2201028:	00f11623          	sh	a5,12(sp)
 220102c:	00010723          	sb	zero,14(sp)
 2201030:	56a000e7          	jalr	1386(zero) # 0 <__DYNAMIC>
 2201034:	00849793          	slli	a5,s1,0x8
 2201038:	3c84b48b          	extu	s1,s1,15,8
 220103c:	8cdd                	or	s1,s1,a5
 220103e:	4681                	li	a3,0
 2201040:	47c1                	li	a5,16
 2201042:	4601                	li	a2,0
 2201044:	4591                	li	a1,4
 2201046:	0068                	addi	a0,sp,12
 2201048:	00f11623          	sh	a5,12(sp)
 220104c:	00911723          	sh	s1,14(sp)
 2201050:	56a000e7          	jalr	1386(zero) # 0 <__DYNAMIC>
 2201054:	77f1                	lui	a5,0xffffc
 2201056:	07bd                	addi	a5,a5,15
 2201058:	0007a437          	lui	s0,0x7a
 220105c:	00f11823          	sh	a5,16(sp)
 2201060:	00010a23          	sb	zero,20(sp)
 2201064:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 2201068:	a801                	j	2201078 <hook_snand_mark_bad+0xb8>
 220106a:	01414783          	lbu	a5,20(sp)
 220106e:	0017f713          	andi	a4,a5,1
 2201072:	c31d                	beqz	a4,2201098 <hook_snand_mark_bad+0xd8>
 2201074:	02040e63          	beqz	s0,22010b0 <hook_snand_mark_bad+0xf0>
 2201078:	4685                	li	a3,1
 220107a:	0850                	addi	a2,sp,20
 220107c:	4589                	li	a1,2
 220107e:	0808                	addi	a0,sp,16
 2201080:	fde00097          	auipc	ra,0xfde00
 2201084:	df0080e7          	jalr	-528(ra) # e70 <sfc_read_reg>
 2201088:	147d                	addi	s0,s0,-1
 220108a:	d165                	beqz	a0,220106a <hook_snand_mark_bad+0xaa>
 220108c:	557d                	li	a0,-1
 220108e:	50b2                	lw	ra,44(sp)
 2201090:	5422                	lw	s0,40(sp)
 2201092:	5492                	lw	s1,36(sp)
 2201094:	6145                	addi	sp,sp,48
 2201096:	8082                	ret
 2201098:	50b2                	lw	ra,44(sp)
 220109a:	5422                	lw	s0,40(sp)
 220109c:	a5fbb737          	lui	a4,0xa5fbb
 22010a0:	8bb1                	andi	a5,a5,12
 22010a2:	dfe70713          	addi	a4,a4,-514 # a5fbadfe <__end+0xa3d7c446>
 22010a6:	5492                	lw	s1,36(sp)
 22010a8:	42f7150b          	mvnez	a0,a4,a5
 22010ac:	6145                	addi	sp,sp,48
 22010ae:	8082                	ret
 22010b0:	50b2                	lw	ra,44(sp)
 22010b2:	5422                	lw	s0,40(sp)
 22010b4:	5492                	lw	s1,36(sp)
 22010b6:	f8c00513          	li	a0,-116
 22010ba:	6145                	addi	sp,sp,48
 22010bc:	8082                	ret
 22010be:	5529                	li	a0,-22
 22010c0:	8082                	ret

022010c2 <hook_snand_chk_bad>:
 22010c2:	080007b7          	lui	a5,0x8000
 22010c6:	06f57663          	bgeu	a0,a5,2201132 <hook_snand_chk_bad+0x70>
 22010ca:	1101                	addi	sp,sp,-32
 22010cc:	77f1                	lui	a5,0xffffc
 22010ce:	cc22                	sw	s0,24(sp)
 22010d0:	07bd                	addi	a5,a5,15
 22010d2:	0007a437          	lui	s0,0x7a
 22010d6:	ca26                	sw	s1,20(sp)
 22010d8:	ce06                	sw	ra,28(sp)
 22010da:	84aa                	mv	s1,a0
 22010dc:	00f11423          	sh	a5,8(sp)
 22010e0:	00010623          	sb	zero,12(sp)
 22010e4:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 22010e8:	a039                	j	22010f6 <hook_snand_chk_bad+0x34>
 22010ea:	00c14783          	lbu	a5,12(sp)
 22010ee:	8b85                	andi	a5,a5,1
 22010f0:	cf99                	beqz	a5,220110e <hook_snand_chk_bad+0x4c>
 22010f2:	00040c63          	beqz	s0,220110a <hook_snand_chk_bad+0x48>
 22010f6:	4685                	li	a3,1
 22010f8:	0070                	addi	a2,sp,12
 22010fa:	4589                	li	a1,2
 22010fc:	0028                	addi	a0,sp,8
 22010fe:	fde00097          	auipc	ra,0xfde00
 2201102:	d72080e7          	jalr	-654(ra) # e70 <sfc_read_reg>
 2201106:	147d                	addi	s0,s0,-1
 2201108:	d16d                	beqz	a0,22010ea <hook_snand_chk_bad+0x28>
 220110a:	5541                	li	a0,-16
 220110c:	a831                	j	2201128 <hook_snand_chk_bad+0x66>
 220110e:	8526                	mv	a0,s1
 2201110:	fde00097          	auipc	ra,0xfde00
 2201114:	5c0080e7          	jalr	1472(ra) # 16d0 <sfc_nand_chk_blk>
 2201118:	a5fbb7b7          	lui	a5,0xa5fbb
 220111c:	00950713          	addi	a4,a0,9
 2201120:	dfd78793          	addi	a5,a5,-515 # a5fbadfd <__end+0xa3d7c445>
 2201124:	40e7950b          	mveqz	a0,a5,a4
 2201128:	40f2                	lw	ra,28(sp)
 220112a:	4462                	lw	s0,24(sp)
 220112c:	44d2                	lw	s1,20(sp)
 220112e:	6105                	addi	sp,sp,32
 2201130:	8082                	ret
 2201132:	5529                	li	a0,-22
 2201134:	8082                	ret

02201136 <hook_snand_enable_bp>:
 2201136:	77e9                	lui	a5,0xffffa
 2201138:	1101                	addi	sp,sp,-32
 220113a:	07fd                	addi	a5,a5,31
 220113c:	00f11623          	sh	a5,12(sp)
 2201140:	0068                	addi	a0,sp,12
 2201142:	03800793          	li	a5,56
 2201146:	458d                	li	a1,3
 2201148:	ce06                	sw	ra,28(sp)
 220114a:	00f10723          	sb	a5,14(sp)
 220114e:	fde00097          	auipc	ra,0xfde00
 2201152:	d58080e7          	jalr	-680(ra) # ea6 <sfc_write_reg>
 2201156:	40f2                	lw	ra,28(sp)
 2201158:	6105                	addi	sp,sp,32
 220115a:	8082                	ret

0220115c <hook_snand_disable_bp>:
 220115c:	1101                	addi	sp,sp,-32
 220115e:	77e9                	lui	a5,0xffffa
 2201160:	0068                	addi	a0,sp,12
 2201162:	07fd                	addi	a5,a5,31
 2201164:	458d                	li	a1,3
 2201166:	ce06                	sw	ra,28(sp)
 2201168:	00f11623          	sh	a5,12(sp)
 220116c:	00010723          	sb	zero,14(sp)
 2201170:	fde00097          	auipc	ra,0xfde00
 2201174:	d36080e7          	jalr	-714(ra) # ea6 <sfc_write_reg>
 2201178:	40f2                	lw	ra,28(sp)
 220117a:	6105                	addi	sp,sp,32
 220117c:	8082                	ret

0220117e <hook_erase_blk>:
 220117e:	1101                	addi	sp,sp,-32
 2201180:	77f1                	lui	a5,0xffffc
 2201182:	cc22                	sw	s0,24(sp)
 2201184:	07bd                	addi	a5,a5,15
 2201186:	0007a437          	lui	s0,0x7a
 220118a:	ca26                	sw	s1,20(sp)
 220118c:	ce06                	sw	ra,28(sp)
 220118e:	84aa                	mv	s1,a0
 2201190:	00f11423          	sh	a5,8(sp)
 2201194:	00010623          	sb	zero,12(sp)
 2201198:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 220119c:	a039                	j	22011aa <hook_erase_blk+0x2c>
 220119e:	00c14783          	lbu	a5,12(sp)
 22011a2:	8b85                	andi	a5,a5,1
 22011a4:	c39d                	beqz	a5,22011ca <hook_erase_blk+0x4c>
 22011a6:	0a040c63          	beqz	s0,220125e <hook_erase_blk+0xe0>
 22011aa:	4685                	li	a3,1
 22011ac:	0070                	addi	a2,sp,12
 22011ae:	4589                	li	a1,2
 22011b0:	0028                	addi	a0,sp,8
 22011b2:	fde00097          	auipc	ra,0xfde00
 22011b6:	cbe080e7          	jalr	-834(ra) # e70 <sfc_read_reg>
 22011ba:	147d                	addi	s0,s0,-1
 22011bc:	d16d                	beqz	a0,220119e <hook_erase_blk+0x20>
 22011be:	40f2                	lw	ra,28(sp)
 22011c0:	4462                	lw	s0,24(sp)
 22011c2:	44d2                	lw	s1,20(sp)
 22011c4:	557d                	li	a0,-1
 22011c6:	6105                	addi	sp,sp,32
 22011c8:	8082                	ret
 22011ca:	7469                	lui	s0,0xffffa
 22011cc:	047d                	addi	s0,s0,31
 22011ce:	458d                	li	a1,3
 22011d0:	0068                	addi	a0,sp,12
 22011d2:	00811623          	sh	s0,12(sp)
 22011d6:	00010723          	sb	zero,14(sp)
 22011da:	fde00097          	auipc	ra,0xfde00
 22011de:	ccc080e7          	jalr	-820(ra) # ea6 <sfc_write_reg>
 22011e2:	460d                	li	a2,3
 22011e4:	00b4d593          	srli	a1,s1,0xb
 22011e8:	0d800513          	li	a0,216
 22011ec:	fde00097          	auipc	ra,0xfde00
 22011f0:	f3a080e7          	jalr	-198(ra) # 1126 <lib_sfc_erase>
 22011f4:	03800793          	li	a5,56
 22011f8:	458d                	li	a1,3
 22011fa:	0068                	addi	a0,sp,12
 22011fc:	00811623          	sh	s0,12(sp)
 2201200:	00f10723          	sb	a5,14(sp)
 2201204:	fde00097          	auipc	ra,0xfde00
 2201208:	ca2080e7          	jalr	-862(ra) # ea6 <sfc_write_reg>
 220120c:	77f1                	lui	a5,0xffffc
 220120e:	07bd                	addi	a5,a5,15
 2201210:	0007a437          	lui	s0,0x7a
 2201214:	00f11423          	sh	a5,8(sp)
 2201218:	00010623          	sb	zero,12(sp)
 220121c:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 2201220:	a801                	j	2201230 <hook_erase_blk+0xb2>
 2201222:	00c14783          	lbu	a5,12(sp)
 2201226:	0017f713          	andi	a4,a5,1
 220122a:	cf11                	beqz	a4,2201246 <hook_erase_blk+0xc8>
 220122c:	02040963          	beqz	s0,220125e <hook_erase_blk+0xe0>
 2201230:	4685                	li	a3,1
 2201232:	0070                	addi	a2,sp,12
 2201234:	4589                	li	a1,2
 2201236:	0028                	addi	a0,sp,8
 2201238:	fde00097          	auipc	ra,0xfde00
 220123c:	c38080e7          	jalr	-968(ra) # e70 <sfc_read_reg>
 2201240:	147d                	addi	s0,s0,-1
 2201242:	d165                	beqz	a0,2201222 <hook_erase_blk+0xa4>
 2201244:	bfad                	j	22011be <hook_erase_blk+0x40>
 2201246:	40f2                	lw	ra,28(sp)
 2201248:	4462                	lw	s0,24(sp)
 220124a:	a5fbb737          	lui	a4,0xa5fbb
 220124e:	8bb1                	andi	a5,a5,12
 2201250:	dfe70713          	addi	a4,a4,-514 # a5fbadfe <__end+0xa3d7c446>
 2201254:	44d2                	lw	s1,20(sp)
 2201256:	42f7150b          	mvnez	a0,a4,a5
 220125a:	6105                	addi	sp,sp,32
 220125c:	8082                	ret
 220125e:	40f2                	lw	ra,28(sp)
 2201260:	4462                	lw	s0,24(sp)
 2201262:	44d2                	lw	s1,20(sp)
 2201264:	f8c00513          	li	a0,-116
 2201268:	6105                	addi	sp,sp,32
 220126a:	8082                	ret

0220126c <hook_snand_set_ecc_thr>:
 220126c:	ffc50713          	addi	a4,a0,-4
 2201270:	478d                	li	a5,3
 2201272:	00e7e863          	bltu	a5,a4,2201282 <hook_snand_set_ecc_thr+0x16>
 2201276:	022027b7          	lui	a5,0x2202
 220127a:	cea7a023          	sw	a0,-800(a5) # 2201ce0 <sst_ctx>
 220127e:	4501                	li	a0,0
 2201280:	8082                	ret
 2201282:	557d                	li	a0,-1
 2201284:	8082                	ret

02201286 <hook_snand_get_ecc_thr>:
 2201286:	022027b7          	lui	a5,0x2202
 220128a:	ce07a503          	lw	a0,-800(a5) # 2201ce0 <sst_ctx>
 220128e:	8082                	ret

02201290 <snand_read_page>:
 2201290:	6785                	lui	a5,0x1
 2201292:	00d58733          	add	a4,a1,a3
 2201296:	88078793          	addi	a5,a5,-1920 # 880 <ssbus_drv_write+0xa6>
 220129a:	0ae7e263          	bltu	a5,a4,220133e <snand_read_page+0xae>
 220129e:	7179                	addi	sp,sp,-48
 22012a0:	3c85370b          	extu	a4,a0,15,8
 22012a4:	00851793          	slli	a5,a0,0x8
 22012a8:	d422                	sw	s0,40(sp)
 22012aa:	d226                	sw	s1,36(sp)
 22012ac:	d04a                	sw	s2,32(sp)
 22012ae:	8fd9                	or	a5,a5,a4
 22012b0:	842a                	mv	s0,a0
 22012b2:	01055713          	srli	a4,a0,0x10
 22012b6:	8936                	mv	s2,a3
 22012b8:	84ae                	mv	s1,a1
 22012ba:	46cd                	li	a3,19
 22012bc:	4591                	li	a1,4
 22012be:	0028                	addi	a0,sp,8
 22012c0:	ce4e                	sw	s3,28(sp)
 22012c2:	d606                	sw	ra,44(sp)
 22012c4:	89b2                	mv	s3,a2
 22012c6:	00d10423          	sb	a3,8(sp)
 22012ca:	00e104a3          	sb	a4,9(sp)
 22012ce:	00f11523          	sh	a5,10(sp)
 22012d2:	fde00097          	auipc	ra,0xfde00
 22012d6:	bd4080e7          	jalr	-1068(ra) # ea6 <sfc_write_reg>
 22012da:	4509                	li	a0,2
 22012dc:	36fd                	jal	2200eca <hook_snand_get_status>
 22012de:	c105                	beqz	a0,22012fe <snand_read_page+0x6e>
 22012e0:	a5fbb7b7          	lui	a5,0xa5fbb
 22012e4:	dff78793          	addi	a5,a5,-513 # a5fbadff <__end+0xa3d7c447>
 22012e8:	04f51463          	bne	a0,a5,2201330 <snand_read_page+0xa0>
 22012ec:	022027b7          	lui	a5,0x2202
 22012f0:	ce078793          	addi	a5,a5,-800 # 2201ce0 <sst_ctx>
 22012f4:	4798                	lw	a4,8(a5)
 22012f6:	4685                	li	a3,1
 22012f8:	c3d4                	sw	a3,4(a5)
 22012fa:	e311                	bnez	a4,22012fe <snand_read_page+0x6e>
 22012fc:	c780                	sw	s0,8(a5)
 22012fe:	4601                	li	a2,0
 2201300:	4581                	li	a1,0
 2201302:	4509                	li	a0,2
 2201304:	408000e7          	jalr	1032(zero) # 0 <__DYNAMIC>
 2201308:	5c84b78b          	extu	a5,s1,23,8
 220130c:	03b00713          	li	a4,59
 2201310:	86ca                	mv	a3,s2
 2201312:	864e                	mv	a2,s3
 2201314:	4589                	li	a1,2
 2201316:	0028                	addi	a0,sp,8
 2201318:	00e105a3          	sb	a4,11(sp)
 220131c:	00f106a3          	sb	a5,13(sp)
 2201320:	00910723          	sb	s1,14(sp)
 2201324:	000107a3          	sb	zero,15(sp)
 2201328:	46c000e7          	jalr	1132(zero) # 0 <__DYNAMIC>
 220132c:	40a9150b          	mveqz	a0,s2,a0
 2201330:	50b2                	lw	ra,44(sp)
 2201332:	5422                	lw	s0,40(sp)
 2201334:	5492                	lw	s1,36(sp)
 2201336:	5902                	lw	s2,32(sp)
 2201338:	49f2                	lw	s3,28(sp)
 220133a:	6145                	addi	sp,sp,48
 220133c:	8082                	ret
 220133e:	5529                	li	a0,-22
 2201340:	8082                	ret
	...

02201344 <snand_rw_blk>:
 2201344:	711d                	addi	sp,sp,-96
 2201346:	ce86                	sw	ra,92(sp)
 2201348:	cca2                	sw	s0,88(sp)
 220134a:	caa6                	sw	s1,84(sp)
 220134c:	c8ca                	sw	s2,80(sp)
 220134e:	c6ce                	sw	s3,76(sp)
 2201350:	c4d2                	sw	s4,72(sp)
 2201352:	c2d6                	sw	s5,68(sp)
 2201354:	c0da                	sw	s6,64(sp)
 2201356:	de5e                	sw	s7,60(sp)
 2201358:	dc62                	sw	s8,56(sp)
 220135a:	da66                	sw	s9,52(sp)
 220135c:	d86a                	sw	s10,48(sp)
 220135e:	d66e                	sw	s11,44(sp)
 2201360:	c42a                	sw	a0,8(sp)
 2201362:	080007b7          	lui	a5,0x8000
 2201366:	22f57763          	bgeu	a0,a5,2201594 <snand_rw_blk+0x250>
 220136a:	7ff57d93          	andi	s11,a0,2047
 220136e:	00cd8733          	add	a4,s11,a2
 2201372:	000207b7          	lui	a5,0x20
 2201376:	84b2                	mv	s1,a2
 2201378:	8b36                	mv	s6,a3
 220137a:	00b55913          	srli	s2,a0,0xb
 220137e:	20e7eb63          	bltu	a5,a4,2201594 <snand_rw_blk+0x250>
 2201382:	77f1                	lui	a5,0xffffc
 2201384:	07bd                	addi	a5,a5,15
 2201386:	0007a437          	lui	s0,0x7a
 220138a:	89ae                	mv	s3,a1
 220138c:	00f11c23          	sh	a5,24(sp)
 2201390:	00010e23          	sb	zero,28(sp)
 2201394:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 2201398:	a039                	j	22013a6 <snand_rw_blk+0x62>
 220139a:	01c14783          	lbu	a5,28(sp)
 220139e:	8b85                	andi	a5,a5,1
 22013a0:	cf9d                	beqz	a5,22013de <snand_rw_blk+0x9a>
 22013a2:	00040d63          	beqz	s0,22013bc <snand_rw_blk+0x78>
 22013a6:	4685                	li	a3,1
 22013a8:	0870                	addi	a2,sp,28
 22013aa:	4589                	li	a1,2
 22013ac:	0828                	addi	a0,sp,24
 22013ae:	fde00097          	auipc	ra,0xfde00
 22013b2:	ac2080e7          	jalr	-1342(ra) # e70 <sfc_read_reg>
 22013b6:	872a                	mv	a4,a0
 22013b8:	147d                	addi	s0,s0,-1
 22013ba:	d165                	beqz	a0,220139a <snand_rw_blk+0x56>
 22013bc:	5741                	li	a4,-16
 22013be:	40f6                	lw	ra,92(sp)
 22013c0:	4466                	lw	s0,88(sp)
 22013c2:	44d6                	lw	s1,84(sp)
 22013c4:	4946                	lw	s2,80(sp)
 22013c6:	49b6                	lw	s3,76(sp)
 22013c8:	4a26                	lw	s4,72(sp)
 22013ca:	4a96                	lw	s5,68(sp)
 22013cc:	4b06                	lw	s6,64(sp)
 22013ce:	5bf2                	lw	s7,60(sp)
 22013d0:	5c62                	lw	s8,56(sp)
 22013d2:	5cd2                	lw	s9,52(sp)
 22013d4:	5d42                	lw	s10,48(sp)
 22013d6:	5db2                	lw	s11,44(sp)
 22013d8:	853a                	mv	a0,a4
 22013da:	6125                	addi	sp,sp,96
 22013dc:	8082                	ret
 22013de:	4785                	li	a5,1
 22013e0:	18fb0763          	beq	s6,a5,220156e <snand_rw_blk+0x22a>
 22013e4:	fc048de3          	beqz	s1,22013be <snand_rw_blk+0x7a>
 22013e8:	6785                	lui	a5,0x1
 22013ea:	80078793          	addi	a5,a5,-2048 # 800 <ssbus_drv_write+0x26>
 22013ee:	0097b733          	sltu	a4,a5,s1
 22013f2:	843e                	mv	s0,a5
 22013f4:	40e4940b          	mveqz	s0,s1,a4
 22013f8:	008d86b3          	add	a3,s11,s0
 22013fc:	41b78733          	sub	a4,a5,s11
 2201400:	00d7b6b3          	sltu	a3,a5,a3
 2201404:	87ba                	mv	a5,a4
 2201406:	6a05                	lui	s4,0x1
 2201408:	40d4178b          	mveqz	a5,s0,a3
 220140c:	7bf1                	lui	s7,0xffffc
 220140e:	880a0c13          	addi	s8,s4,-1920 # 880 <ssbus_drv_write+0xa6>
 2201412:	43b7940b          	mvnez	s0,a5,s11
 2201416:	4a81                	li	s5,0
 2201418:	4d09                	li	s10,2
 220141a:	4cc1                	li	s9,16
 220141c:	0bbd                	addi	s7,s7,15
 220141e:	800a0a13          	addi	s4,s4,-2048
 2201422:	020b1863          	bnez	s6,2201452 <snand_rw_blk+0x10e>
 2201426:	0001                	nop
 2201428:	86a2                	mv	a3,s0
 220142a:	864e                	mv	a2,s3
 220142c:	85ee                	mv	a1,s11
 220142e:	854a                	mv	a0,s2
 2201430:	3585                	jal	2201290 <snand_read_page>
 2201432:	872a                	mv	a4,a0
 2201434:	10851f63          	bne	a0,s0,2201552 <snand_rw_blk+0x20e>
 2201438:	8c81                	sub	s1,s1,s0
 220143a:	0905                	addi	s2,s2,1
 220143c:	99a2                	add	s3,s3,s0
 220143e:	9aa2                	add	s5,s5,s0
 2201440:	c4e5                	beqz	s1,2201528 <snand_rw_blk+0x1e4>
 2201442:	009a37b3          	sltu	a5,s4,s1
 2201446:	8452                	mv	s0,s4
 2201448:	40f4940b          	mveqz	s0,s1,a5
 220144c:	4d81                	li	s11,0
 220144e:	fc0b0de3          	beqz	s6,2201428 <snand_rw_blk+0xe4>
 2201452:	01b407b3          	add	a5,s0,s11
 2201456:	12fc6d63          	bltu	s8,a5,2201590 <snand_rw_blk+0x24c>
 220145a:	fde00097          	auipc	ra,0xfde00
 220145e:	ca6080e7          	jalr	-858(ra) # 1100 <lib_sfc_write_en>
 2201462:	008dd793          	srli	a5,s11,0x8
 2201466:	86a2                	mv	a3,s0
 2201468:	864e                	mv	a2,s3
 220146a:	458d                	li	a1,3
 220146c:	0848                	addi	a0,sp,20
 220146e:	00f10aa3          	sb	a5,21(sp)
 2201472:	01b10b23          	sb	s11,22(sp)
 2201476:	01a10a23          	sb	s10,20(sp)
 220147a:	56a000e7          	jalr	1386(zero) # 0 <__DYNAMIC>
 220147e:	3c89370b          	extu	a4,s2,15,8
 2201482:	00891793          	slli	a5,s2,0x8
 2201486:	8fd9                	or	a5,a5,a4
 2201488:	4681                	li	a3,0
 220148a:	01095713          	srli	a4,s2,0x10
 220148e:	4601                	li	a2,0
 2201490:	4591                	li	a1,4
 2201492:	0848                	addi	a0,sp,20
 2201494:	00f11b23          	sh	a5,22(sp)
 2201498:	01910a23          	sb	s9,20(sp)
 220149c:	00e10aa3          	sb	a4,21(sp)
 22014a0:	56a000e7          	jalr	1386(zero) # 0 <__DYNAMIC>
 22014a4:	0007a7b7          	lui	a5,0x7a
 22014a8:	01711c23          	sh	s7,24(sp)
 22014ac:	00010e23          	sb	zero,28(sp)
 22014b0:	12078d93          	addi	s11,a5,288 # 7a120 <__text_end+0x76118>
 22014b4:	a801                	j	22014c4 <snand_rw_blk+0x180>
 22014b6:	01c14703          	lbu	a4,28(sp)
 22014ba:	00177693          	andi	a3,a4,1
 22014be:	cab9                	beqz	a3,2201514 <snand_rw_blk+0x1d0>
 22014c0:	060d8163          	beqz	s11,2201522 <snand_rw_blk+0x1de>
 22014c4:	4685                	li	a3,1
 22014c6:	0870                	addi	a2,sp,28
 22014c8:	4589                	li	a1,2
 22014ca:	0828                	addi	a0,sp,24
 22014cc:	fde00097          	auipc	ra,0xfde00
 22014d0:	9a4080e7          	jalr	-1628(ra) # e70 <sfc_read_reg>
 22014d4:	1dfd                	addi	s11,s11,-1
 22014d6:	d165                	beqz	a0,22014b6 <snand_rw_blk+0x172>
 22014d8:	577d                	li	a4,-1
 22014da:	77e9                	lui	a5,0xffffa
 22014dc:	07fd                	addi	a5,a5,31
 22014de:	458d                	li	a1,3
 22014e0:	00f11e23          	sh	a5,28(sp)
 22014e4:	0868                	addi	a0,sp,28
 22014e6:	03800793          	li	a5,56
 22014ea:	c63a                	sw	a4,12(sp)
 22014ec:	00f10f23          	sb	a5,30(sp)
 22014f0:	fde00097          	auipc	ra,0xfde00
 22014f4:	9b6080e7          	jalr	-1610(ra) # ea6 <sfc_write_reg>
 22014f8:	4732                	lw	a4,12(sp)
 22014fa:	4622                	lw	a2,8(sp)
 22014fc:	022025b7          	lui	a1,0x2202
 2201500:	86ba                	mv	a3,a4
 2201502:	bf458593          	addi	a1,a1,-1036 # 2201bf4 <__cmdl_end+0x70>
 2201506:	4511                	li	a0,4
 2201508:	fde01097          	auipc	ra,0xfde01
 220150c:	396080e7          	jalr	918(ra) # 289e <ts_log>
 2201510:	4732                	lw	a4,12(sp)
 2201512:	b575                	j	22013be <snand_rw_blk+0x7a>
 2201514:	8b31                	andi	a4,a4,12
 2201516:	d30d                	beqz	a4,2201438 <snand_rw_blk+0xf4>
 2201518:	a5fbb737          	lui	a4,0xa5fbb
 220151c:	dfe70713          	addi	a4,a4,-514 # a5fbadfe <__end+0xa3d7c446>
 2201520:	bf6d                	j	22014da <snand_rw_blk+0x196>
 2201522:	f8c00713          	li	a4,-116
 2201526:	bf55                	j	22014da <snand_rw_blk+0x196>
 2201528:	4785                	li	a5,1
 220152a:	8756                	mv	a4,s5
 220152c:	e8fb19e3          	bne	s6,a5,22013be <snand_rw_blk+0x7a>
 2201530:	77e9                	lui	a5,0xffffa
 2201532:	07fd                	addi	a5,a5,31
 2201534:	00f11e23          	sh	a5,28(sp)
 2201538:	458d                	li	a1,3
 220153a:	03800793          	li	a5,56
 220153e:	0868                	addi	a0,sp,28
 2201540:	c43a                	sw	a4,8(sp)
 2201542:	00f10f23          	sb	a5,30(sp)
 2201546:	fde00097          	auipc	ra,0xfde00
 220154a:	960080e7          	jalr	-1696(ra) # ea6 <sfc_write_reg>
 220154e:	4722                	lw	a4,8(sp)
 2201550:	b5bd                	j	22013be <snand_rw_blk+0x7a>
 2201552:	4622                	lw	a2,8(sp)
 2201554:	022025b7          	lui	a1,0x2202
 2201558:	86aa                	mv	a3,a0
 220155a:	c62a                	sw	a0,12(sp)
 220155c:	bd858593          	addi	a1,a1,-1064 # 2201bd8 <__cmdl_end+0x54>
 2201560:	4511                	li	a0,4
 2201562:	fde01097          	auipc	ra,0xfde01
 2201566:	33c080e7          	jalr	828(ra) # 289e <ts_log>
 220156a:	4732                	lw	a4,12(sp)
 220156c:	bd89                	j	22013be <snand_rw_blk+0x7a>
 220156e:	77e9                	lui	a5,0xffffa
 2201570:	07fd                	addi	a5,a5,31
 2201572:	458d                	li	a1,3
 2201574:	0868                	addi	a0,sp,28
 2201576:	c63a                	sw	a4,12(sp)
 2201578:	00f11e23          	sh	a5,28(sp)
 220157c:	00010f23          	sb	zero,30(sp)
 2201580:	fde00097          	auipc	ra,0xfde00
 2201584:	926080e7          	jalr	-1754(ra) # ea6 <sfc_write_reg>
 2201588:	4732                	lw	a4,12(sp)
 220158a:	e4049fe3          	bnez	s1,22013e8 <snand_rw_blk+0xa4>
 220158e:	b74d                	j	2201530 <snand_rw_blk+0x1ec>
 2201590:	5729                	li	a4,-22
 2201592:	b7a1                	j	22014da <snand_rw_blk+0x196>
 2201594:	5729                	li	a4,-22
 2201596:	b525                	j	22013be <snand_rw_blk+0x7a>
	...

0220159a <hook_snand_read_blk>:
 220159a:	4681                	li	a3,0
 220159c:	b365                	j	2201344 <snand_rw_blk>

0220159e <hook_snand_write_blk>:
 220159e:	4685                	li	a3,1
 22015a0:	b355                	j	2201344 <snand_rw_blk>

022015a2 <hook_snand_write>:
 22015a2:	7139                	addi	sp,sp,-64
 22015a4:	de06                	sw	ra,60(sp)
 22015a6:	dc22                	sw	s0,56(sp)
 22015a8:	da26                	sw	s1,52(sp)
 22015aa:	d84a                	sw	s2,48(sp)
 22015ac:	d64e                	sw	s3,44(sp)
 22015ae:	d452                	sw	s4,40(sp)
 22015b0:	d256                	sw	s5,36(sp)
 22015b2:	d05a                	sw	s6,32(sp)
 22015b4:	ce5e                	sw	s7,28(sp)
 22015b6:	cc62                	sw	s8,24(sp)
 22015b8:	00c50733          	add	a4,a0,a2
 22015bc:	080007b7          	lui	a5,0x8000
 22015c0:	0ce7e463          	bltu	a5,a4,2201688 <hook_snand_write+0xe6>
 22015c4:	89ae                	mv	s3,a1
 22015c6:	c1e9                	beqz	a1,2201688 <hook_snand_write+0xe6>
 22015c8:	77f1                	lui	a5,0xffffc
 22015ca:	07bd                	addi	a5,a5,15
 22015cc:	0007a437          	lui	s0,0x7a
 22015d0:	892a                	mv	s2,a0
 22015d2:	84b2                	mv	s1,a2
 22015d4:	8a36                	mv	s4,a3
 22015d6:	00f11423          	sh	a5,8(sp)
 22015da:	00010623          	sb	zero,12(sp)
 22015de:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 22015e2:	a039                	j	22015f0 <hook_snand_write+0x4e>
 22015e4:	00c14783          	lbu	a5,12(sp)
 22015e8:	8b85                	andi	a5,a5,1
 22015ea:	cb9d                	beqz	a5,2201620 <hook_snand_write+0x7e>
 22015ec:	00040c63          	beqz	s0,2201604 <hook_snand_write+0x62>
 22015f0:	4685                	li	a3,1
 22015f2:	0070                	addi	a2,sp,12
 22015f4:	4589                	li	a1,2
 22015f6:	0028                	addi	a0,sp,8
 22015f8:	fde00097          	auipc	ra,0xfde00
 22015fc:	878080e7          	jalr	-1928(ra) # e70 <sfc_read_reg>
 2201600:	147d                	addi	s0,s0,-1
 2201602:	d16d                	beqz	a0,22015e4 <hook_snand_write+0x42>
 2201604:	4c01                	li	s8,0
 2201606:	50f2                	lw	ra,60(sp)
 2201608:	5462                	lw	s0,56(sp)
 220160a:	54d2                	lw	s1,52(sp)
 220160c:	5942                	lw	s2,48(sp)
 220160e:	59b2                	lw	s3,44(sp)
 2201610:	5a22                	lw	s4,40(sp)
 2201612:	5a92                	lw	s5,36(sp)
 2201614:	5b02                	lw	s6,32(sp)
 2201616:	4bf2                	lw	s7,28(sp)
 2201618:	8562                	mv	a0,s8
 220161a:	4c62                	lw	s8,24(sp)
 220161c:	6121                	addi	sp,sp,64
 220161e:	8082                	ret
 2201620:	00020ab7          	lui	s5,0x20
 2201624:	1afd                	addi	s5,s5,-1
 2201626:	01597ab3          	and	s5,s2,s5
 220162a:	dce9                	beqz	s1,2201604 <hook_snand_write+0x62>
 220162c:	4b81                	li	s7,0
 220162e:	4c01                	li	s8,0
 2201630:	00020b37          	lui	s6,0x20
 2201634:	854a                	mv	a0,s2
 2201636:	fde00097          	auipc	ra,0xfde00
 220163a:	09a080e7          	jalr	154(ra) # 16d0 <sfc_nand_chk_blk>
 220163e:	009b37b3          	sltu	a5,s6,s1
 2201642:	8426                	mv	s0,s1
 2201644:	42fb140b          	mvnez	s0,s6,a5
 2201648:	01540733          	add	a4,s0,s5
 220164c:	415b07b3          	sub	a5,s6,s5
 2201650:	00eb3733          	sltu	a4,s6,a4
 2201654:	40e4178b          	mveqz	a5,s0,a4
 2201658:	882a                	mv	a6,a0
 220165a:	4357940b          	mvnez	s0,a5,s5
 220165e:	85ce                	mv	a1,s3
 2201660:	854a                	mv	a0,s2
 2201662:	4685                	li	a3,1
 2201664:	8622                	mv	a2,s0
 2201666:	00080763          	beqz	a6,2201674 <hook_snand_write+0xd2>
 220166a:	0b85                	addi	s7,s7,1
 220166c:	f97a6de3          	bltu	s4,s7,2201606 <hook_snand_write+0x64>
 2201670:	995a                	add	s2,s2,s6
 2201672:	b7c9                	j	2201634 <hook_snand_write+0x92>
 2201674:	39c1                	jal	2201344 <snand_rw_blk>
 2201676:	8c81                	sub	s1,s1,s0
 2201678:	00851a63          	bne	a0,s0,220168c <hook_snand_write+0xea>
 220167c:	9922                	add	s2,s2,s0
 220167e:	99a2                	add	s3,s3,s0
 2201680:	9c22                	add	s8,s8,s0
 2201682:	d0d1                	beqz	s1,2201606 <hook_snand_write+0x64>
 2201684:	4a81                	li	s5,0
 2201686:	b77d                	j	2201634 <hook_snand_write+0x92>
 2201688:	5c29                	li	s8,-22
 220168a:	bfb5                	j	2201606 <hook_snand_write+0x64>
 220168c:	8c2a                	mv	s8,a0
 220168e:	bfa5                	j	2201606 <hook_snand_write+0x64>

02201690 <hook_snand_read>:
 2201690:	7139                	addi	sp,sp,-64
 2201692:	de06                	sw	ra,60(sp)
 2201694:	dc22                	sw	s0,56(sp)
 2201696:	da26                	sw	s1,52(sp)
 2201698:	d84a                	sw	s2,48(sp)
 220169a:	d64e                	sw	s3,44(sp)
 220169c:	d452                	sw	s4,40(sp)
 220169e:	d256                	sw	s5,36(sp)
 22016a0:	d05a                	sw	s6,32(sp)
 22016a2:	ce5e                	sw	s7,28(sp)
 22016a4:	cc62                	sw	s8,24(sp)
 22016a6:	00c50733          	add	a4,a0,a2
 22016aa:	080007b7          	lui	a5,0x8000
 22016ae:	0ce7ec63          	bltu	a5,a4,2201786 <hook_snand_read+0xf6>
 22016b2:	89ae                	mv	s3,a1
 22016b4:	c9e9                	beqz	a1,2201786 <hook_snand_read+0xf6>
 22016b6:	77f1                	lui	a5,0xffffc
 22016b8:	07bd                	addi	a5,a5,15
 22016ba:	0007a437          	lui	s0,0x7a
 22016be:	892a                	mv	s2,a0
 22016c0:	84b2                	mv	s1,a2
 22016c2:	00f11423          	sh	a5,8(sp)
 22016c6:	00010623          	sb	zero,12(sp)
 22016ca:	12040413          	addi	s0,s0,288 # 7a120 <__text_end+0x76118>
 22016ce:	a039                	j	22016dc <hook_snand_read+0x4c>
 22016d0:	00c14783          	lbu	a5,12(sp)
 22016d4:	8b85                	andi	a5,a5,1
 22016d6:	cb9d                	beqz	a5,220170c <hook_snand_read+0x7c>
 22016d8:	00040c63          	beqz	s0,22016f0 <hook_snand_read+0x60>
 22016dc:	4685                	li	a3,1
 22016de:	0070                	addi	a2,sp,12
 22016e0:	4589                	li	a1,2
 22016e2:	0028                	addi	a0,sp,8
 22016e4:	fddff097          	auipc	ra,0xfddff
 22016e8:	78c080e7          	jalr	1932(ra) # e70 <sfc_read_reg>
 22016ec:	147d                	addi	s0,s0,-1
 22016ee:	d16d                	beqz	a0,22016d0 <hook_snand_read+0x40>
 22016f0:	4c01                	li	s8,0
 22016f2:	50f2                	lw	ra,60(sp)
 22016f4:	5462                	lw	s0,56(sp)
 22016f6:	54d2                	lw	s1,52(sp)
 22016f8:	5942                	lw	s2,48(sp)
 22016fa:	59b2                	lw	s3,44(sp)
 22016fc:	5a22                	lw	s4,40(sp)
 22016fe:	5a92                	lw	s5,36(sp)
 2201700:	5b02                	lw	s6,32(sp)
 2201702:	4bf2                	lw	s7,28(sp)
 2201704:	8562                	mv	a0,s8
 2201706:	4c62                	lw	s8,24(sp)
 2201708:	6121                	addi	sp,sp,64
 220170a:	8082                	ret
 220170c:	022027b7          	lui	a5,0x2202
 2201710:	ce078793          	addi	a5,a5,-800 # 2201ce0 <sst_ctx>
 2201714:	00020a37          	lui	s4,0x20
 2201718:	1a7d                	addi	s4,s4,-1
 220171a:	0007a223          	sw	zero,4(a5)
 220171e:	0007a423          	sw	zero,8(a5)
 2201722:	01497a33          	and	s4,s2,s4
 2201726:	d4e9                	beqz	s1,22016f0 <hook_snand_read+0x60>
 2201728:	4b01                	li	s6,0
 220172a:	4c01                	li	s8,0
 220172c:	00020ab7          	lui	s5,0x20
 2201730:	4b91                	li	s7,4
 2201732:	854a                	mv	a0,s2
 2201734:	fde00097          	auipc	ra,0xfde00
 2201738:	f9c080e7          	jalr	-100(ra) # 16d0 <sfc_nand_chk_blk>
 220173c:	009ab7b3          	sltu	a5,s5,s1
 2201740:	8426                	mv	s0,s1
 2201742:	42fa940b          	mvnez	s0,s5,a5
 2201746:	01440733          	add	a4,s0,s4
 220174a:	414a87b3          	sub	a5,s5,s4
 220174e:	00eab733          	sltu	a4,s5,a4
 2201752:	40e4178b          	mveqz	a5,s0,a4
 2201756:	882a                	mv	a6,a0
 2201758:	4347940b          	mvnez	s0,a5,s4
 220175c:	85ce                	mv	a1,s3
 220175e:	854a                	mv	a0,s2
 2201760:	4681                	li	a3,0
 2201762:	8622                	mv	a2,s0
 2201764:	00080763          	beqz	a6,2201772 <hook_snand_read+0xe2>
 2201768:	0b05                	addi	s6,s6,1
 220176a:	f96be4e3          	bltu	s7,s6,22016f2 <hook_snand_read+0x62>
 220176e:	9956                	add	s2,s2,s5
 2201770:	b7c9                	j	2201732 <hook_snand_read+0xa2>
 2201772:	3ec9                	jal	2201344 <snand_rw_blk>
 2201774:	8c81                	sub	s1,s1,s0
 2201776:	00851a63          	bne	a0,s0,220178a <hook_snand_read+0xfa>
 220177a:	9922                	add	s2,s2,s0
 220177c:	99a2                	add	s3,s3,s0
 220177e:	9c22                	add	s8,s8,s0
 2201780:	d8ad                	beqz	s1,22016f2 <hook_snand_read+0x62>
 2201782:	4a01                	li	s4,0
 2201784:	b77d                	j	2201732 <hook_snand_read+0xa2>
 2201786:	5c29                	li	s8,-22
 2201788:	b7ad                	j	22016f2 <hook_snand_read+0x62>
 220178a:	8c2a                	mv	s8,a0
 220178c:	b79d                	j	22016f2 <hook_snand_read+0x62>

0220178e <hook_snand_get_ecc_flg>:
 220178e:	022027b7          	lui	a5,0x2202
 2201792:	ce47a503          	lw	a0,-796(a5) # 2201ce4 <sst_ctx+0x4>
 2201796:	8082                	ret

02201798 <hook_snand_get_risk_addr>:
 2201798:	022027b7          	lui	a5,0x2202
 220179c:	ce87a503          	lw	a0,-792(a5) # 2201ce8 <sst_ctx+0x8>
 22017a0:	8082                	ret

022017a2 <hook_boot_app>:
 22017a2:	7179                	addi	sp,sp,-48
 22017a4:	d422                	sw	s0,40(sp)
 22017a6:	e1d2c737          	lui	a4,0xe1d2c
 22017aa:	02202437          	lui	s0,0x2202
 22017ae:	cec40413          	addi	s0,s0,-788 # 2201cec <sst_bt_ctx>
 22017b2:	4785                	li	a5,1
 22017b4:	3b470713          	addi	a4,a4,948 # e1d2c3b4 <__end+0xdfaed9fc>
 22017b8:	c05c                	sw	a5,4(s0)
 22017ba:	c45c                	sw	a5,12(s0)
 22017bc:	c81c                	sw	a5,16(s0)
 22017be:	d606                	sw	ra,44(sp)
 22017c0:	d226                	sw	s1,36(sp)
 22017c2:	d04a                	sw	s2,32(sp)
 22017c4:	ce4e                	sw	s3,28(sp)
 22017c6:	cc52                	sw	s4,24(sp)
 22017c8:	ca56                	sw	s5,20(sp)
 22017ca:	c85a                	sw	s6,16(sp)
 22017cc:	c65e                	sw	s7,12(sp)
 22017ce:	c462                	sw	s8,8(sp)
 22017d0:	c266                	sw	s9,4(sp)
 22017d2:	c06a                	sw	s10,0(sp)
 22017d4:	c018                	sw	a4,0(s0)
 22017d6:	00042423          	sw	zero,8(s0)
 22017da:	fde00097          	auipc	ra,0xfde00
 22017de:	858080e7          	jalr	-1960(ra) # 1032 <lib_sfc_scan>
 22017e2:	001607b7          	lui	a5,0x160
 22017e6:	22a7a023          	sw	a0,544(a5) # 160220 <__text_end+0x15c218>
 22017ea:	4789                	li	a5,2
 22017ec:	10f50263          	beq	a0,a5,22018f0 <hook_boot_app+0x14e>
 22017f0:	478d                	li	a5,3
 22017f2:	06f51b63          	bne	a0,a5,2201868 <hook_boot_app+0xc6>
 22017f6:	022014b7          	lui	s1,0x2201
 22017fa:	69048493          	addi	s1,s1,1680 # 2201690 <hook_snand_read>
 22017fe:	4058                	lw	a4,4(s0)
 2201800:	4785                	li	a5,1
 2201802:	08f70163          	beq	a4,a5,2201884 <hook_boot_app+0xe2>
 2201806:	022029b7          	lui	s3,0x2202
 220180a:	02202a37          	lui	s4,0x2202
 220180e:	4b09                	li	s6,2
 2201810:	00180937          	lui	s2,0x180
 2201814:	d1498993          	addi	s3,s3,-748 # 2201d14 <sst_bt_ctx+0x28>
 2201818:	d1ca0a13          	addi	s4,s4,-740 # 2201d1c <sst_bt_ctx+0x30>
 220181c:	02202c37          	lui	s8,0x2202
 2201820:	4a85                	li	s5,1
 2201822:	4b89                	li	s7,2
 2201824:	04000613          	li	a2,64
 2201828:	85ce                	mv	a1,s3
 220182a:	854a                	mv	a0,s2
 220182c:	9482                	jalr	s1
 220182e:	872a                	mv	a4,a0
 2201830:	04000793          	li	a5,64
 2201834:	03800613          	li	a2,56
 2201838:	85d2                	mv	a1,s4
 220183a:	4501                	li	a0,0
 220183c:	00f71e63          	bne	a4,a5,2201858 <hook_boot_app+0xb6>
 2201840:	fde01097          	auipc	ra,0xfde01
 2201844:	1a8080e7          	jalr	424(ra) # 29e8 <verify_lib_calc_crc32>
 2201848:	545c                	lw	a5,44(s0)
 220184a:	872a                	mv	a4,a0
 220184c:	864a                	mv	a2,s2
 220184e:	c10c0593          	addi	a1,s8,-1008 # 2201c10 <__cmdl_end+0x8c>
 2201852:	451d                	li	a0,7
 2201854:	04f70f63          	beq	a4,a5,22018b2 <hook_boot_app+0x110>
 2201858:	405c                	lw	a5,4(s0)
 220185a:	05578463          	beq	a5,s5,22018a2 <hook_boot_app+0x100>
 220185e:	01542223          	sw	s5,4(s0)
 2201862:	4785                	li	a5,1
 2201864:	02fb1b63          	bne	s6,a5,220189a <hook_boot_app+0xf8>
 2201868:	02202637          	lui	a2,0x2202
 220186c:	022025b7          	lui	a1,0x2202
 2201870:	c4c60613          	addi	a2,a2,-948 # 2201c4c <__FUNCTION__.0>
 2201874:	c3458593          	addi	a1,a1,-972 # 2201c34 <__cmdl_end+0xb0>
 2201878:	4511                	li	a0,4
 220187a:	fde01097          	auipc	ra,0xfde01
 220187e:	024080e7          	jalr	36(ra) # 289e <ts_log>
 2201882:	a001                	j	2201882 <hook_boot_app+0xe0>
 2201884:	022029b7          	lui	s3,0x2202
 2201888:	02202a37          	lui	s4,0x2202
 220188c:	4789                	li	a5,2
 220188e:	d1498993          	addi	s3,s3,-748 # 2201d14 <sst_bt_ctx+0x28>
 2201892:	d1ca0a13          	addi	s4,s4,-740 # 2201d1c <sst_bt_ctx+0x30>
 2201896:	02202c37          	lui	s8,0x2202
 220189a:	8b3e                	mv	s6,a5
 220189c:	00080937          	lui	s2,0x80
 22018a0:	b741                	j	2201820 <hook_boot_app+0x7e>
 22018a2:	01742223          	sw	s7,4(s0)
 22018a6:	00180937          	lui	s2,0x180
 22018aa:	fb5b0fe3          	beq	s6,s5,2201868 <hook_boot_app+0xc6>
 22018ae:	4b05                	li	s6,1
 22018b0:	bf95                	j	2201824 <hook_boot_app+0x82>
 22018b2:	03c42c83          	lw	s9,60(s0)
 22018b6:	86e6                	mv	a3,s9
 22018b8:	fde01097          	auipc	ra,0xfde01
 22018bc:	fe6080e7          	jalr	-26(ra) # 289e <ts_log>
 22018c0:	03442d03          	lw	s10,52(s0)
 22018c4:	85e6                	mv	a1,s9
 22018c6:	04090513          	addi	a0,s2,64 # 180040 <__text_end+0x17c038>
 22018ca:	866a                	mv	a2,s10
 22018cc:	9482                	jalr	s1
 22018ce:	87aa                	mv	a5,a0
 22018d0:	866a                	mv	a2,s10
 22018d2:	85e6                	mv	a1,s9
 22018d4:	4501                	li	a0,0
 22018d6:	f8fd11e3          	bne	s10,a5,2201858 <hook_boot_app+0xb6>
 22018da:	fde01097          	auipc	ra,0xfde01
 22018de:	10e080e7          	jalr	270(ra) # 29e8 <verify_lib_calc_crc32>
 22018e2:	403c                	lw	a5,64(s0)
 22018e4:	f6f51ae3          	bne	a0,a5,2201858 <hook_boot_app+0xb6>
 22018e8:	03942223          	sw	s9,36(s0)
 22018ec:	9c82                	jalr	s9
 22018ee:	a001                	j	22018ee <hook_boot_app+0x14c>
 22018f0:	6485                	lui	s1,0x1
 22018f2:	23c48493          	addi	s1,s1,572 # 123c <lib_sfc_nor_read>
 22018f6:	b721                	j	22017fe <hook_boot_app+0x5c>

022018f8 <bt_param_write>:
 22018f8:	715d                	addi	sp,sp,-80
 22018fa:	da56                	sw	s5,52(sp)
 22018fc:	8aaa                	mv	s5,a0
 22018fe:	4501                	li	a0,0
 2201900:	d65e                	sw	s7,44(sp)
 2201902:	c686                	sw	ra,76(sp)
 2201904:	c4a2                	sw	s0,72(sp)
 2201906:	c2a6                	sw	s1,68(sp)
 2201908:	c0ca                	sw	s2,64(sp)
 220190a:	de4e                	sw	s3,60(sp)
 220190c:	dc52                	sw	s4,56(sp)
 220190e:	d85a                	sw	s6,48(sp)
 2201910:	8bae                	mv	s7,a1
 2201912:	db8ff0ef          	jal	ra,2200eca <hook_snand_get_status>
 2201916:	e959                	bnez	a0,22019ac <bt_param_write+0xb4>
 2201918:	6985                	lui	s3,0x1
 220191a:	00020937          	lui	s2,0x20
 220191e:	8b2a                	mv	s6,a0
 2201920:	8456                	mv	s0,s5
 2201922:	83bff0ef          	jal	ra,220115c <hook_snand_disable_bp>
 2201926:	5a7d                	li	s4,-1
 2201928:	80098993          	addi	s3,s3,-2048 # 800 <ssbus_drv_write+0x26>
 220192c:	9956                	add	s2,s2,s5
 220192e:	0001                	nop
 2201930:	02000613          	li	a2,32
 2201934:	858a                	mv	a1,sp
 2201936:	8522                	mv	a0,s0
 2201938:	3ba1                	jal	2201690 <hook_snand_read>
 220193a:	02000793          	li	a5,32
 220193e:	84aa                	mv	s1,a0
 2201940:	02f51763          	bne	a0,a5,220196e <bt_param_write+0x76>
 2201944:	4782                	lw	a5,0(sp)
 2201946:	05478863          	beq	a5,s4,2201996 <bt_param_write+0x9e>
 220194a:	944e                	add	s0,s0,s3
 220194c:	ff2462e3          	bltu	s0,s2,2201930 <bt_param_write+0x38>
 2201950:	8556                	mv	a0,s5
 2201952:	82dff0ef          	jal	ra,220117e <hook_erase_blk>
 2201956:	e91d                	bnez	a0,220198c <bt_param_write+0x94>
 2201958:	8456                	mv	s0,s5
 220195a:	02000613          	li	a2,32
 220195e:	858a                	mv	a1,sp
 2201960:	8522                	mv	a0,s0
 2201962:	333d                	jal	2201690 <hook_snand_read>
 2201964:	02000793          	li	a5,32
 2201968:	84aa                	mv	s1,a0
 220196a:	fcf50de3          	beq	a0,a5,2201944 <bt_param_write+0x4c>
 220196e:	fc8ff0ef          	jal	ra,2201136 <hook_snand_enable_bp>
 2201972:	5b7d                	li	s6,-1
 2201974:	40b6                	lw	ra,76(sp)
 2201976:	4426                	lw	s0,72(sp)
 2201978:	4496                	lw	s1,68(sp)
 220197a:	4906                	lw	s2,64(sp)
 220197c:	59f2                	lw	s3,60(sp)
 220197e:	5a62                	lw	s4,56(sp)
 2201980:	5ad2                	lw	s5,52(sp)
 2201982:	5bb2                	lw	s7,44(sp)
 2201984:	855a                	mv	a0,s6
 2201986:	5b42                	lw	s6,48(sp)
 2201988:	6161                	addi	sp,sp,80
 220198a:	8082                	ret
 220198c:	8522                	mv	a0,s0
 220198e:	e32ff0ef          	jal	ra,2200fc0 <hook_snand_mark_bad>
 2201992:	8456                	mv	s0,s5
 2201994:	bf71                	j	2201930 <bt_param_write+0x38>
 2201996:	4685                	li	a3,1
 2201998:	02000613          	li	a2,32
 220199c:	85de                	mv	a1,s7
 220199e:	8522                	mv	a0,s0
 22019a0:	3109                	jal	22015a2 <hook_snand_write>
 22019a2:	fc9516e3          	bne	a0,s1,220196e <bt_param_write+0x76>
 22019a6:	f90ff0ef          	jal	ra,2201136 <hook_snand_enable_bp>
 22019aa:	b7e9                	j	2201974 <bt_param_write+0x7c>
 22019ac:	5b41                	li	s6,-16
 22019ae:	b7d9                	j	2201974 <bt_param_write+0x7c>
	...

022019b2 <get_bt_param>:
 22019b2:	7131                	addi	sp,sp,-192
 22019b4:	d74e                	sw	s3,172(sp)
 22019b6:	000e07b7          	lui	a5,0xe0
 22019ba:	6985                	lui	s3,0x1
 22019bc:	dd22                	sw	s0,184(sp)
 22019be:	d94a                	sw	s2,176(sp)
 22019c0:	d552                	sw	s4,168(sp)
 22019c2:	d356                	sw	s5,164(sp)
 22019c4:	d15a                	sw	s6,160(sp)
 22019c6:	cd62                	sw	s8,152(sp)
 22019c8:	cb66                	sw	s9,148(sp)
 22019ca:	c96a                	sw	s10,144(sp)
 22019cc:	c76e                	sw	s11,140(sp)
 22019ce:	df06                	sw	ra,188(sp)
 22019d0:	db26                	sw	s1,180(sp)
 22019d2:	cf5e                	sw	s7,156(sp)
 22019d4:	8daa                	mv	s11,a0
 22019d6:	ca3e                	sw	a5,20(sp)
 22019d8:	01810a93          	addi	s5,sp,24
 22019dc:	04010a13          	addi	s4,sp,64
 22019e0:	01010b13          	addi	s6,sp,16
 22019e4:	1000                	addi	s0,sp,32
 22019e6:	000a0d37          	lui	s10,0xa0
 22019ea:	7c01                	lui	s8,0xfffe0
 22019ec:	597d                	li	s2,-1
 22019ee:	00020cb7          	lui	s9,0x20
 22019f2:	80098993          	addi	s3,s3,-2048 # 800 <ssbus_drv_write+0x26>
 22019f6:	4501                	li	a0,0
 22019f8:	000aa023          	sw	zero,0(s5) # 20000 <__text_end+0x1bff8>
 22019fc:	012a2023          	sw	s2,0(s4)
 2201a00:	ccaff0ef          	jal	ra,2200eca <hook_snand_get_status>
 2201a04:	018d7bb3          	and	s7,s10,s8
 2201a08:	e50d                	bnez	a0,2201a32 <get_bt_param+0x80>
 2201a0a:	019b84b3          	add	s1,s7,s9
 2201a0e:	009d6963          	bltu	s10,s1,2201a20 <get_bt_param+0x6e>
 2201a12:	a005                	j	2201a32 <get_bt_param+0x80>
 2201a14:	5682                	lw	a3,32(sp)
 2201a16:	09268263          	beq	a3,s2,2201a9a <get_bt_param+0xe8>
 2201a1a:	9d4e                	add	s10,s10,s3
 2201a1c:	009d7b63          	bgeu	s10,s1,2201a32 <get_bt_param+0x80>
 2201a20:	856a                	mv	a0,s10
 2201a22:	02000613          	li	a2,32
 2201a26:	85a2                	mv	a1,s0
 2201a28:	31a5                	jal	2201690 <hook_snand_read>
 2201a2a:	02000693          	li	a3,32
 2201a2e:	fed503e3          	beq	a0,a3,2201a14 <get_bt_param+0x62>
 2201a32:	0a91                	addi	s5,s5,4
 2201a34:	020a0a13          	addi	s4,s4,32
 2201a38:	0b11                	addi	s6,s6,4
 2201a3a:	01540563          	beq	s0,s5,2201a44 <get_bt_param+0x92>
 2201a3e:	000b2d03          	lw	s10,0(s6) # 20000 <__text_end+0x1bff8>
 2201a42:	bf55                	j	22019f6 <get_bt_param+0x44>
 2201a44:	47e2                	lw	a5,24(sp)
 2201a46:	4705                	li	a4,1
 2201a48:	46f2                	lw	a3,28(sp)
 2201a4a:	0ae78163          	beq	a5,a4,2201aec <get_bt_param+0x13a>
 2201a4e:	8fd5                	or	a5,a5,a3
 2201a50:	c3f9                	beqz	a5,2201b16 <get_bt_param+0x164>
 2201a52:	108c                	addi	a1,sp,96
 2201a54:	02000613          	li	a2,32
 2201a58:	856e                	mv	a0,s11
 2201a5a:	fde00097          	auipc	ra,0xfde00
 2201a5e:	18e080e7          	jalr	398(ra) # 1be8 <ts_memcpy>
 2201a62:	108c                	addi	a1,sp,96
 2201a64:	000a0537          	lui	a0,0xa0
 2201a68:	3d41                	jal	22018f8 <bt_param_write>
 2201a6a:	02202537          	lui	a0,0x2202
 2201a6e:	c9450513          	addi	a0,a0,-876 # 2201c94 <__FUNCTION__.0+0x48>
 2201a72:	fde01097          	auipc	ra,0xfde01
 2201a76:	db8080e7          	jalr	-584(ra) # 282a <printf>
 2201a7a:	4501                	li	a0,0
 2201a7c:	50fa                	lw	ra,188(sp)
 2201a7e:	546a                	lw	s0,184(sp)
 2201a80:	54da                	lw	s1,180(sp)
 2201a82:	594a                	lw	s2,176(sp)
 2201a84:	59ba                	lw	s3,172(sp)
 2201a86:	5a2a                	lw	s4,168(sp)
 2201a88:	5a9a                	lw	s5,164(sp)
 2201a8a:	5b0a                	lw	s6,160(sp)
 2201a8c:	4bfa                	lw	s7,156(sp)
 2201a8e:	4c6a                	lw	s8,152(sp)
 2201a90:	4cda                	lw	s9,148(sp)
 2201a92:	4d4a                	lw	s10,144(sp)
 2201a94:	4dba                	lw	s11,140(sp)
 2201a96:	6129                	addi	sp,sp,192
 2201a98:	8082                	ret
 2201a9a:	c62a                	sw	a0,12(sp)
 2201a9c:	800d0513          	addi	a0,s10,-2048 # 9f800 <__text_end+0x9b7f8>
 2201aa0:	f97569e3          	bltu	a0,s7,2201a32 <get_bt_param+0x80>
 2201aa4:	02000613          	li	a2,32
 2201aa8:	85d2                	mv	a1,s4
 2201aaa:	36dd                	jal	2201690 <hook_snand_read>
 2201aac:	4732                	lw	a4,12(sp)
 2201aae:	f8e512e3          	bne	a0,a4,2201a32 <get_bt_param+0x80>
 2201ab2:	014a2b83          	lw	s7,20(s4)
 2201ab6:	02000613          	li	a2,32
 2201aba:	85d2                	mv	a1,s4
 2201abc:	4501                	li	a0,0
 2201abe:	000a2a23          	sw	zero,20(s4)
 2201ac2:	fde01097          	auipc	ra,0xfde01
 2201ac6:	f26080e7          	jalr	-218(ra) # 29e8 <verify_lib_calc_crc32>
 2201aca:	022027b7          	lui	a5,0x2202
 2201ace:	84aa                	mv	s1,a0
 2201ad0:	85aa                	mv	a1,a0
 2201ad2:	865e                	mv	a2,s7
 2201ad4:	c5c78513          	addi	a0,a5,-932 # 2201c5c <__FUNCTION__.0+0x10>
 2201ad8:	fde01097          	auipc	ra,0xfde01
 2201adc:	d52080e7          	jalr	-686(ra) # 282a <printf>
 2201ae0:	f49b99e3          	bne	s7,s1,2201a32 <get_bt_param+0x80>
 2201ae4:	4785                	li	a5,1
 2201ae6:	00faa023          	sw	a5,0(s5)
 2201aea:	b7a1                	j	2201a32 <get_bt_param+0x80>
 2201aec:	02000613          	li	a2,32
 2201af0:	008c                	addi	a1,sp,64
 2201af2:	856e                	mv	a0,s11
 2201af4:	00f68b63          	beq	a3,a5,2201b0a <get_bt_param+0x158>
 2201af8:	fde00097          	auipc	ra,0xfde00
 2201afc:	0f0080e7          	jalr	240(ra) # 1be8 <ts_memcpy>
 2201b00:	008c                	addi	a1,sp,64
 2201b02:	000e0537          	lui	a0,0xe0
 2201b06:	3bcd                	jal	22018f8 <bt_param_write>
 2201b08:	b78d                	j	2201a6a <get_bt_param+0xb8>
 2201b0a:	fde00097          	auipc	ra,0xfde00
 2201b0e:	0de080e7          	jalr	222(ra) # 1be8 <ts_memcpy>
 2201b12:	4501                	li	a0,0
 2201b14:	b7a5                	j	2201a7c <get_bt_param+0xca>
 2201b16:	02202537          	lui	a0,0x2202
 2201b1a:	c8050513          	addi	a0,a0,-896 # 2201c80 <__FUNCTION__.0+0x34>
 2201b1e:	fde01097          	auipc	ra,0xfde01
 2201b22:	d0c080e7          	jalr	-756(ra) # 282a <printf>
 2201b26:	557d                	li	a0,-1
 2201b28:	bf91                	j	2201a7c <get_bt_param+0xca>

02201b2a <set_bt_param>:
 2201b2a:	1141                	addi	sp,sp,-16
 2201b2c:	c606                	sw	ra,12(sp)
 2201b2e:	c422                	sw	s0,8(sp)
 2201b30:	c226                	sw	s1,4(sp)
 2201b32:	842a                	mv	s0,a0
 2201b34:	85aa                	mv	a1,a0
 2201b36:	02000613          	li	a2,32
 2201b3a:	00052a23          	sw	zero,20(a0)
 2201b3e:	4501                	li	a0,0
 2201b40:	fde01097          	auipc	ra,0xfde01
 2201b44:	ea8080e7          	jalr	-344(ra) # 29e8 <verify_lib_calc_crc32>
 2201b48:	862a                	mv	a2,a0
 2201b4a:	022025b7          	lui	a1,0x2202
 2201b4e:	c850                	sw	a2,20(s0)
 2201b50:	451d                	li	a0,7
 2201b52:	cac58593          	addi	a1,a1,-852 # 2201cac <__FUNCTION__.0+0x60>
 2201b56:	fde01097          	auipc	ra,0xfde01
 2201b5a:	d48080e7          	jalr	-696(ra) # 289e <ts_log>
 2201b5e:	85a2                	mv	a1,s0
 2201b60:	000a0537          	lui	a0,0xa0
 2201b64:	3b51                	jal	22018f8 <bt_param_write>
 2201b66:	84aa                	mv	s1,a0
 2201b68:	85a2                	mv	a1,s0
 2201b6a:	000e0537          	lui	a0,0xe0
 2201b6e:	3369                	jal	22018f8 <bt_param_write>
 2201b70:	40b2                	lw	ra,12(sp)
 2201b72:	4422                	lw	s0,8(sp)
 2201b74:	8d45                	or	a0,a0,s1
 2201b76:	4492                	lw	s1,4(sp)
 2201b78:	0141                	addi	sp,sp,16
 2201b7a:	8082                	ret

02201b7c <cmdl_reg_reboot>:
 2201b7c:	1bd0 0220 0e6c 0220                         .. .l. .

02201b84 <__cmdl_end>:
 2201b84:	6376                	flw	ft6,92(sp)
 2201b86:	6173206f          	j	223499c <__hook_end+0x32c44>
 2201b8a:	6566                	flw	fa0,88(sp)
 2201b8c:	6320                	flw	fs0,64(a4)
 2201b8e:	6f6c                	flw	fa1,92(a4)
 2201b90:	000a6573          	csrrsi	a0,ustatus,20
 2201b94:	4c50                	lw	a2,28(s0)
 2201b96:	204c                	fld	fa1,128(s0)
 2201b98:	6f6c                	flw	fa1,92(a4)
 2201b9a:	74206b63          	bltu	zero,sp,22022f0 <__hook_end+0x598>
 2201b9e:	6d69                	lui	s10,0x1a
 2201ba0:	6f65                	lui	t5,0x19
 2201ba2:	7475                	lui	s0,0xffffd
 2201ba4:	000a                	c.slli	zero,0x2
 2201ba6:	0000                	unimp
 2201ba8:	253c                	fld	fa5,72(a0)
 2201baa:	3e64                	fld	fs1,248(a2)
 2201bac:	3a20                	fld	fs0,112(a2)
 2201bae:	7020                	flw	fs0,96(s0)
 2201bb0:	6372                	flw	ft6,28(sp)
 2201bb2:	6920                	flw	fs0,80(a0)
 2201bb4:	696e                	flw	fs2,216(sp)
 2201bb6:	2074                	fld	fa3,192(s0)
 2201bb8:	6166                	flw	ft2,88(sp)
 2201bba:	6c69                	lui	s8,0x1a
 2201bbc:	000a                	c.slli	zero,0x2
 2201bbe:	0000                	unimp
 2201bc0:	7270                	flw	fa2,100(a2)
 2201bc2:	6e692063          	0x6e692063
 2201bc6:	7469                	lui	s0,0xffffa
 2201bc8:	6420                	flw	fs0,72(s0)
 2201bca:	0a656e6f          	jal	t3,2257c70 <__end+0x192b8>
 2201bce:	0000                	unimp
 2201bd0:	6572                	flw	fa0,28(sp)
 2201bd2:	6f62                	flw	ft10,24(sp)
 2201bd4:	0000746f          	jal	s0,2208bd4 <__hook_end+0x6e7c>
 2201bd8:	2325                	jal	2202100 <__hook_end+0x3a8>
 2201bda:	2078                	fld	fa4,192(s0)
 2201bdc:	6572                	flw	fa0,28(sp)
 2201bde:	6461                	lui	s0,0x18
 2201be0:	6620                	flw	fs0,72(a2)
 2201be2:	6961                	lui	s2,0x18
 2201be4:	2c6c                	fld	fa1,216(s0)
 2201be6:	7220                	flw	fs0,96(a2)
 2201be8:	7465                	lui	s0,0xffff9
 2201bea:	253d                	jal	2202218 <__hook_end+0x4c0>
 2201bec:	2864                	fld	fs1,208(s0)
 2201bee:	7825                	lui	a6,0xfffe9
 2201bf0:	0a29                	addi	s4,s4,10
 2201bf2:	0000                	unimp
 2201bf4:	2325                	jal	220211c <__hook_end+0x3c4>
 2201bf6:	2078                	fld	fa4,192(s0)
 2201bf8:	74697277          	0x74697277
 2201bfc:	2065                	jal	2201ca4 <__FUNCTION__.0+0x58>
 2201bfe:	6166                	flw	ft2,88(sp)
 2201c00:	6c69                	lui	s8,0x1a
 2201c02:	202c                	fld	fa1,64(s0)
 2201c04:	6572                	flw	fa0,28(sp)
 2201c06:	3d74                	fld	fa3,248(a0)
 2201c08:	6425                	lui	s0,0x9
 2201c0a:	2528                	fld	fa0,72(a0)
 2201c0c:	2978                	fld	fa4,208(a0)
 2201c0e:	000a                	c.slli	zero,0x2
 2201c10:	7274                	flw	fa3,100(a2)
 2201c12:	2079                	jal	2201ca0 <__FUNCTION__.0+0x54>
 2201c14:	6f62                	flw	ft10,24(sp)
 2201c16:	6620746f          	jal	s0,2209278 <__hook_end+0x7520>
 2201c1a:	6f72                	flw	ft10,28(sp)
 2201c1c:	206d                	jal	2201cc6 <__hook_text_end+0x2>
 2201c1e:	6c66                	flw	fs8,88(sp)
 2201c20:	7361                	lui	t1,0xffff8
 2201c22:	2868                	fld	fa0,208(s0)
 2201c24:	7825                	lui	a6,0xfffe9
 2201c26:	2029                	jal	2201c30 <__cmdl_end+0xac>
 2201c28:	6f74                	flw	fa3,92(a4)
 2201c2a:	7220                	flw	fs0,96(a2)
 2201c2c:	6d61                	lui	s10,0x18
 2201c2e:	2528                	fld	fa0,72(a0)
 2201c30:	2978                	fld	fa4,208(a0)
 2201c32:	000a                	c.slli	zero,0x2
 2201c34:	7325                	lui	t1,0xfffe9
 2201c36:	693a                	flw	fs2,140(sp)
 2201c38:	676d                	lui	a4,0x1b
 2201c3a:	6320                	flw	fs0,64(a4)
 2201c3c:	6e61                	lui	t3,0x18
 2201c3e:	62207427          	0x62207427
 2201c42:	75746f6f          	jal	t5,2248b98 <__end+0xa1e0>
 2201c46:	0a70                	addi	a2,sp,284
 2201c48:	0000                	unimp
	...

02201c4c <__FUNCTION__.0>:
 2201c4c:	6f68 6b6f 625f 6f6f 5f74 7061 0070 0000     hook_boot_app...
 2201c5c:	6572 6461 7020 7261 6d61 6320 6c61 2063     read param calc 
 2201c6c:	7263 2063 7825 7220 6165 2064 7263 2063     crc %x read crc 
 2201c7c:	7825 000a 7263 2063 6166 6c69 6420 756f     %x..crc fail dou
 2201c8c:	6c62 0a65 0000 0000 636f 7563 2072 6e6f     ble.....occur on
 2201c9c:	2065 7265 2c72 6e61 2064 7063 0a79 0000     e err,and cpy...
 2201cac:	6573 5f74 7462 705f 7261 6d61 6320 6372     set_bt_param crc
 2201cbc:	2520 7823 000a 0000                          %#x....
