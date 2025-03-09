# Smart Lock System  
ENG5220: Real Time Embedded Programming (2024-25) Group 12 Project

**Team 12**:  
[Peining Liu (2615219L)](https://github.com/Peink), [Hao Zhang (3028132Z)](https://github.com/Hao2777-dot),  
[Mengyang He (3030975H)](https://github.com/MengyangHe666), [Hao Sun (2955851S)](https://github.com/HaoSun-666),  
[Chengyi Lyu (2960858L)](https://github.com/UOGLyucy)


  <img 
    src="https://github.com/user-attachments/assets/1f0445d5-08b7-437e-8585-058f80ac4d8c" 
    alt="Github" 
    width="20"
  />
**GitHub**: [Team12-Real-Time-Embedded-Programming-2024-25](https://github.com/Peink/Team12-Real-Time-Embedded-Programming-2024-25)  


  <img 
    src="https://github.com/user-attachments/assets/0add5f06-649c-413e-aa76-046fde2687fd" 
    alt="Instagram" 
    width="20"
  />
**Instagram**:[uogrtep.t12.2025](https://www.instagram.com/uogrtep.t12.2025/) 


---

## 🚀 Project Overview  
A multi-factor authentication smart lock system combining **facial recognition**, **fingerprint scanning**, and **password verification** for enhanced security. Integrated with remote monitoring, data logging, and real-time alarms, this system is designed for versatile applications in home, office, and industrial environments.  

**Developed using:** C++, Raspberry Pi GPIO, LAN-based communication，[OpenCV](https://opencv.org), etc.

---

## 🌟 Key Features
 
### 🔒 Authentication Methods  
- **Facial Recognition**  
  - Infrared distance camera module for accurate detection and monite remote environment.
- **Fingerprint Lock**  
  - Fingerprint sensor module for biometric verification.  
- **Password Lock**  
  - Matrix button input for numeric key matching.  

### 📡 Remote Monitoring & Control  
- Real-time data collection and time-stamped logging.  
- Remote alarm system (buzzer integration, screen display, and the LAN updates).  
- Motor control for automated locking/unlocking.  

### 🛠️ Hardware Design  
- Custom 3D-printed enclosure for durability and compactness.  
- Integration of sensors, actuators, and embedded modules.  

---

### 🖥️ Sensors, Hardware & Software  
- **Sensors**: Infrared camera, fingerprint sensor, matrix buttons.  
- **Actuators**: Buzzer, motor control.  
- **Software**: Real-time embedded programming for multi-factor authentication and data handling.  

---
## 📌 System Architecture

### **Hardware Components**
- **Raspberry Pi 5** – Central processing unit
- **Infrared Sensors** – Detect distence and monite remote environment
- **Buzzer** – Alerts strangers who stay longtime
- **Camera Module** – Provides live surveillance
- **LAN Interface** – Transmits real-time alerts to connected client applications or website

### **Software Components**
- **Multithreading System**: Manages events in real-time
- **Client Applications or website (LAN-Based)**: Receives alerts and displays monitoring data


## ✨ Practicality & Innovation  

### ✅ Practicality  
- **Multi-factor authentication** for robust security.  
- Remote monitoring and alarm system for real-time alerts.  
- Versatile deployment in diverse environments.  
- Data recording for audit and analysis.  

### 💡 Innovation  
- **Real-time remote detection** and logging capabilities.  
- Hybrid authentication combining biometrics, passwords, and hardware control.  
- Open-source design for customization and scalability.  
---
## 🛠️ How It Works
1. Sensors collect data (like facical, fingerprint,etc) via GPIO.
2. Judge the data is compare with the datas already inside the system, if it's match open lock.
3. Event handlers perform multiple tasks:
   - **Display alert messages on screen**
   - **Trigger a buzzer alarm**
   - **Send alerts to LAN-connected client applications**
   - **Start live video surveillance (if enabled)**
4. Users can remotely monitor sensor readings and alerts via LAN-connected apps.
5. The system operates autonomously with minimal latency
---
## 📦 The Structure of Project
![image](https://github.com/Peink/Team12-Real-Time-Embedded-Programming-2024-25/blob/Project-Document/Project%20Ducument/pic/Structure%20.svg)
---
## ⌚️ The Project Plan

<div align="center">
<img alt="LOGO" src="https://github.com/Peink/Team12-Real-Time-Embedded-Programming-2024-25/blob/Project-Document/Project%20Ducument/pic/Project%20Development%20Timeline.jpg"/>
</div>  


🌟 **Thank you for your interest!**  

