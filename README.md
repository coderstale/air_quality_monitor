# 🌫️ Air Quality Monitor using Arduino + Deep Learning

This project is a smart **Air Quality Monitoring System** built using **Arduino** sensors and a **Keras deep learning model**. It collects real-time environmental data, predicts air pollutant levels, and classifies the Air Quality Index (AQI) bucket (e.g., *Good*, *Unhealthy*, etc.).

> 🔗 Ideal for DIY IoT enthusiasts, environmental monitoring, and machine learning integration on microcontrollers.

---

## 🛠️ Features

- Reads **PM2.5** and **raw air quality** values from Arduino sensors
- Sends data to a Python script via Serial communication
- Uses a trained **Keras model** to predict 5 major pollutants:
  - PM10
  - NO2
  - SO2
  - CO
  - O3
- Classifies the **AQI bucket** using a second neural network head
- Displays prediction on a 16x2 I2C LCD
- Supports **real-time bi-directional communication** between Python and Arduino

---

## 🧠 ML Model Overview

The `.keras` model (`aqi_dual_model_final.keras`) is a dual-output neural network that:

1. Predicts pollutant levels from PM2.5 and AQI values
2. Classifies AQI bucket using softmax

🧪 **Training Data**: Simulated and real-world samples  
🧮 **Scaler**: StandardScaler  
🏷️ **Encoder**: LabelEncoder

---

## 🧩 Project Structure
air_quality_monitor/
├── aqi_dual_model_final.keras   # Trained Keras model

├── air_quality_monitor.ino      # Arduino code for sensor & LCD

├── app.py                 # Python app for ML prediction + serial comm


> Note: Model files like `scaler.pkl` and `label_encoder.pkl` were used during training but are excluded from this upload.

---

## 🚀 Getting Started

### 🖥️ Python Side

1. Connect your Arduino and update the `SERIAL_PORT` in `app.py`
2. Install dependencies:
```pip install tensorflow joblib pyserial numpy```

3.	Run the Python script:
```python app.py```

🤖 Arduino Side
  1.	Flash air_quality_monitor.ino to your Arduino using the Arduino IDE
  2.	Connect your sensors to analog pins A1 and A2
  3.	Ensure I2C LCD is properly wired

⸻

🔌 Communication Protocol
  •	Arduino ➡ Python: DATA:<pm25>,<aqi>
  •	Python ➡ Arduino: BUCKET:<category>

⸻

📦 Large Model Files

⚠️ Some large header files (.h), TensorFlow Lite models, and converted binaries were excluded due to GitHub’s file size limits.

📥 Download missing files here:
Mediafire Model Files

⸻

🧠 Future Improvements
  •	Convert Keras model to .tflite for onboard inference (TinyML)
  •	Add mobile alert integration (e.g., via Bluetooth or SMS)
  •	Use cloud API for continuous data logging and dashboard

⸻

👨‍💻 Author

K Satya Sai Nischal
Computer Science Engineering | Hyderabad, India
GitHub: @coderstale

⸻

📜 License

MIT License — feel free to use, fork, and expand this project!
