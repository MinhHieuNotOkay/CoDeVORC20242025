#include <Adafruit_PWMServoDriver.h> // Thư viện điều khiển PWM cho servo
#include <Wire.h> // Thư viện cho giao tiếp I2C
#include <PS2X_lib.h> // Thư viện cho tay cầm PS2
#include <Adafruit_TCS34725.h> // Thư viện cho cảm biến màu TCS34725

// Định nghĩa chân kết nối với tay cầm PS2
#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

// Định nghĩa các chân kết nối cảm biến
#define SEN_1_PIN 39
#define SEN_2_PIN 36
#define SEN_3_PIN 2
#define SEN_4_PIN 32

// Định nghĩa các chân kết nối động cơ DC
#define DC_MOTOR_1_CHAN_1 10 // Động cơ di chuyển 1, kênh 1
#define DC_MOTOR_1_CHAN_2 11 // Động cơ di chuyển 1, kênh 2
#define DC_MOTOR_2_CHAN_1 8 // Động cơ di chuyển 2, kênh 1
#define DC_MOTOR_2_CHAN_2 9 // Động cơ di chuyển 2, kênh 2
#define DC_MOTOR_3_CHAN_1 12 // Động cơ lấy bóng, kênh 1
#define DC_MOTOR_3_CHAN_2 13 // Động cơ lấy bóng, kênh 2
#define DC_MOTOR_4_CHAN_1 14 // Động cơ bắn bóng, kênh 1
#define DC_MOTOR_4_CHAN_2 15 // Động cơ bắn bóng, kênh 2

// Định nghĩa các chân kết nối servo
#define Servo_180_1 5 // Servo phân loại bóng
#define Servo_180_2 6 // Servo xa bóng đen
#define Servo_360_1 7
#define Servo_360_2 8 // Servo xa bóng trắng

int frequency = 0; // Biến lưu tần số (không được sử dụng trong mã)

#define MIN_SERVO_180_SPEED 204 // Tốc độ quay min của servo 180
#define MAX_SERVO_180_SPEED 410 // Tốc độ quay max của servo 180
#define MIN_SERVO_360_SPEED 220 // Tốc độ min của servo 360
#define MAX_SERVO_360_SPEED 440 // Tốc độ max của servo 360
#define SERVO_FREQ 60 // Tần số servo
#define ENA
#define ENB

#define pressures false
#define rumble false

// Khởi tạo các đối tượng thư viện
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
PS2X ps2x;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

void setup()
{
  pwm.begin(); // Khởi tạo điều khiển PWM
  pwm.setOscillatorFrequency(27000000); // Đặt tần số dao động
  pwm.setPWMFreq(60); // Đặt tần số PWM cho servo
  pwm.setPWM(Servo_180_1, 0,  307); // Đặt giá trị PWM cho Servo 180_1
  pwm.setPWM(Servo_360_1, 0, MAX_SERVO_360_SPEED); // Đặt giá trị PWM cho Servo 360_1

  Wire.setClock(400000); // Đặt tốc độ giao tiếp I2C
  Serial.begin(115200); // Khởi tạo giao tiếp Serial với baud rate 115200
  Serial.print("Ket noi voi tay cam PS2:"); // Thông báo đang kết nối với tay cầm PS2

  int error = -1;
  for (int i = 0; i < 10; i++) // Thử kết nối với tay cầm PS2 tối đa 10 lần
  {
    delay(1000); // Đợi 1 giây
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble); // Cấu hình tay cầm PS2
    Serial.print("."); // Hiển thị dấu chấm trong quá trình kết nối
  }

  switch (error) // Kiểm tra lỗi kết nối
  {
  case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong"); // Kết nối thành công
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam "); // Không tìm thấy tay cầm
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh"); // Không gửi được lệnh
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode "); // Không vào được chế độ Pressures
    break;
  }

  if (tcs.begin())
  {
    Serial.println("Cảm biến hoạt động"); // Cảm biến hoạt động bình thường
  }
  else
  {
    Serial.println("Cảm biến không kết nối, hãy kiểm tra lại kết nối..."); // Cảm biến không kết nối
    while (1); // Dừng chương trình nếu cảm biến không kết nối
  }
}

void banbong() {
   pwm.setPWM(14, 0, 3072); // Điều khiển động cơ bắn bóng
   pwm.setPWM(15, 0, 0);
   Serial.println("PAD_L2 is being held"); // Hiển thị thông báo khi giữ nút L2
}

void laybong() {
   Serial.println("TRIANGLE PRESSED"); // Hiển thị thông báo khi nhấn nút TRIANGLE
   pwm.setPWM(12, 0, 4095); // Điều khiển động cơ lấy bóng
   pwm.setPWM(13, 0, 0);
}

void tuhuy() {
  // Tắt tất cả các động cơ và servo
  pwm.setPWM(8, 0, 0);
  pwm.setPWM(9, 0, 0);
  pwm.setPWM(10, 0, 0);
  pwm.setPWM(11, 0, 0);
  pwm.setPWM(12, 0, 0);
  pwm.setPWM(13, 0, 0);
  pwm.setPWM(14, 0, 0);
  pwm.setPWM(15, 0, 0);
  pwm.setPWM(5, 0, 0);
  pwm.setPWM(6, 0, 0);
  pwm.setPWM(7, 0, 0);
  pwm.setPWM(4, 0, 0);
}

void loop() {
   delay(10); // Đợi 10ms
    ps2x.read_gamepad(false, false); // Đọc dữ liệu từ tay cầm

  int speed = 4095; // Tốc độ động cơ tối đa
  int nJoyX = 128 - ps2x.Analog(PSS_LX); // Đọc giá trị joystick X
  int nJoyY = 128 - ps2x.Analog(PSS_LY); // Đọc giá trị joystick Y
  int nMotMixL; // Đầu ra trộn động cơ trái
  int nMotMixR; // Đầu ra trộn động cơ phải

  nJoyY /= 2; // Chia giá trị joystick Y cho 2
  nJoyX /= 5; // Chia giá trị joystick X cho 5
  bool temp = (nJoyY * nJoyX > 0); // Kiểm tra hướng di chuyển

  if (nJoyX) // Nếu có giá trị joystick X, điều chỉnh động cơ để xoay
  {
    nMotMixL = -nJoyX + (nJoyY * temp);
    nMotMixR = nJoyX + (nJoyY * !temp);
  }
  else // Nếu không có giá trị joystick X, điều chỉnh động cơ để di chuyển tới hoặc lùi
  {
    nMotMixL = nJoyY;
    nMotMixR = nJoyY;
  }

  Serial.print(("Calculated value from joystick: ")); // Hiển thị giá trị tính toán từ joystick
  Serial.print(nMotMixL);
  Serial.print("\t");
  Serial.println(nMotMixR);

  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

  // Điều khiển động cơ di chuyển
  if (nMotMixR >= 0)
  {
    c3 = nMotMixR;
    c3 = map(c3, 0, 127, 0, speed); // Chuyển đổi giá trị cho động cơ
    pwm.setPWM(10, 0, c3); // Điều khiển động cơ di chuyển phải
    pwm.setPWM(11, 0, 0);
  }
  else if (nMotMixR < 0)
  {
    c4 = abs(n
