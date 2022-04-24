#!/usr/bin/env python3


import streamlit as st
import serial, time, serial.tools.list_ports
import sys; sys.stdout.reconfigure(encoding='utf-8');
from datetime import datetime
import gs  # private google sheet py file using gsheet-api library
import pandas as pd
from PIL import Image

image = Image.open('download.jpeg')


st.image(image, caption='meiban')
st.sidebar.markdown("# Recent orders")
gsheet, wsheet= gs.connect() # take note not imported
COMMENT_TEMPLATE_MD = """{} - {}

> weight: {}"""

name_map_coor = {

    "Powder A" : "4,0",
    "Powder B" : "508,850",

}

sys.stdin.reconfigure(encoding='utf-8') 

def detect_ports():
    ports = list(serial.tools.list_ports.comports())
    if(len(ports) == 0):
        print("No ports detected")
    else:
        for i in ports:
            print(i.device) 
    return [port.device for port in ports]

@st.cache(hash_funcs={serial.Serial: id})
def session_state(aux):
    ss= {}
    if 'ser' not in ss:
        ports= detect_ports()
        print(ports)
        ss['ser'] = serial.Serial(ports[1],9600)
        ser = serial.Serial(ports[2],9600)
        ser1 = serial.Serial(ports[1],11520)
        print("initiated serial")
        if(ser.isOpen() ==False):
            ser.open()
            print("open serial")
    return ser, ser1

def main():
    ser , ser1= session_state('aux')
    st.markdown('# Custom recipe')
    choice = st.selectbox("powder", ["Milo","Holick","Mocha","Cocoa"])
    inp = st.text_input('weight')
    submit = st.button("Add ingredient")
    if submit:
        date = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
        wsheet.insert_row([choice, inp, date],2)
    st.write("## Current ingredient:")
    if st.button("Dispense"):
        string =''
        for list in wsheet.get_all_values():
            if list[0] != 'choice':
                name = name_map_coor[list[0]]
                string += name + ',' + list[1] + ','

        string += '1'
        gs.delete_sheet(gsheet,wsheet)
        ser.write(string.encode('utf-8'))
       
    try:
        mydata = wsheet.get_all_records()
        df= pd.DataFrame(mydata)
        st.write(df)
    except:
        st.success('Dispended')



# recent order section
    r1 = st.sidebar.button("order #1234")
    st.sidebar.markdown("Milo 5g, holick 10g")
    st.sidebar.markdown("---")
    r2 = st.sidebar.button("order #1235")
    st.sidebar.markdown("Cocoa 5g, Mocha 8g")
    if r1: 
        st.write('dispensing recipe 1')
        print(ser.isOpen())
        ser.write("1,0,0,4,0,3.00,508,850,2.00,9,0,0".encode('utf-8'))
        line = ''
        while line != "done":
            line = ser.readline().decode()[:4] 
        ser.reset_output_buffer()
        ser1.write("2".encode('utf-8'))

    if r2: 
        st.write('dispensing recipe 1')
        print(ser.isOpen())
        ser.write("1,0,0,4,0,2.00,508,850,2.00,9,0,0".encode('utf-8'))

if name == "__main__":
    main()