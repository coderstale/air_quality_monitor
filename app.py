import serial
import time
import joblib
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import load_model

SERIAL_PORT = "/dev/cu.usbmodem2101"
BAUD_RATE = 115200
MODEL_PATH = "aqi_dual_model_final.keras"
SCALER_PATH = "scaler.pkl"
ENCODER_PATH = "label_encoder.pkl"

print("[INFO] Loading AI model...")
model = load_model(MODEL_PATH)
print("[INFO] Model loaded successfully!")

scaler = joblib.load(SCALER_PATH)
label_encoder = joblib.load(ENCODER_PATH)
print("[INFO] Scaler and Label Encoder loaded successfully!")

while True:
    try:
        print(f"[INFO] Attempting to connect to {SERIAL_PORT} at {BAUD_RATE} baud...")
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
        print(f"[INFO] Connected to {SERIAL_PORT} ✅")
        break
    except serial.SerialException as e:
        print(f"[ERROR] Failed to connect: {e}. Retrying in 5 seconds...")
        time.sleep(5)

def validate_and_parse_data(data):
    if not data.startswith("DATA:"):
        return None  
    try:
        _, values = data.split(":")
        pm25, aqi = map(float, values.split(","))
        return pm25, aqi
    except ValueError:
        print("[ERROR] Data parsing failed! Skipping this iteration.")
        return None

def predict_aqi(pm25, aqi):
    try:
        input_data = np.array([[pm25, aqi]])
        input_data_scaled = scaler.transform(input_data)
        pollutant_preds, aqi_class_pred = model.predict(input_data_scaled)
        aqi_class_index = np.argmax(aqi_class_pred)
        aqi_class = label_encoder.inverse_transform([aqi_class_index])[0]
        return pollutant_preds[0], aqi_class
    except Exception as e:
        print(f"[ERROR] Model prediction failed: {e}")
        return None, "Unknown"

while True:
    try:
        if ser.in_waiting > 0:
            data = ser.readline().decode("utf-8").strip()
            if not data:
                continue
            parsed_values = validate_and_parse_data(data)
            if parsed_values is None:
                continue
            pm25, aqi = parsed_values
            print(f"\n[INFO] Extracted Data - PM2.5: {pm25}, AQI: {aqi}")
            pollutants, aqi_bucket = predict_aqi(pm25, aqi)
            if pollutants is None:
                continue
            print(f"[INFO] Predicted Pollutant Levels:")
            print(f"      - PM10: {pollutants[0]:.2f}")
            print(f"      - NO2: {pollutants[1]:.2f}")
            print(f"      - SO2: {pollutants[2]:.2f}")
            print(f"      - CO: {pollutants[3]:.2f}")
            print(f"      - O3: {pollutants[4]:.2f}")
            print(f"[INFO] Predicted AQI Classification: {aqi_bucket}")
            response = f"BUCKET:{aqi_bucket}\n"
            try:
                ser.write(response.encode("utf-8"))
                ser.flush()
                print(f"[DEBUG] Sent Response to Arduino: {response.strip()}")
            except serial.SerialException as e:
                print(f"[ERROR] Failed to send data to Arduino: {e}")
        time.sleep(0.1)
    except serial.SerialException as e:
        print(f"[ERROR] Serial connection lost: {e}")
        break
    except KeyboardInterrupt:
        print("\n[INFO] Program terminated by user.")
        break
