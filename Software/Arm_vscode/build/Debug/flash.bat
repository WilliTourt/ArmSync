@echo off
echo Flashing RA6M5...
"C:\Users\21181\AppData\Local\Programs\Python\Python310\Scripts\pyocd.exe" flash --target R7FA6M5BF --frequency 100000 --uid 0001A0000001 --erase sector "C:\Users\21181\e2_studio\workspace\06_dshanmcu_ra6m5_lvgl_display_touchpad_encoder_log\Debug\06_dshanmcu_ra6m5_lvgl_display_touchpad_encoder_log.elf"
echo Done.
pause
