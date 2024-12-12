#90% AI generated (syntax), 10% self generated
with open('breastfeeding_model.h5', 'w') as header_file:
    header_file.write("const unsigned char model[] = {\n")
    with open("model.tflite", "rb") as tflite_file:
        byte = tflite_file.read(1)
        while byte:
            header_file.write(f"0x{byte.hex()}, ")
            byte = tflite_file.read(1)
        header_file.write("\n};\n")
