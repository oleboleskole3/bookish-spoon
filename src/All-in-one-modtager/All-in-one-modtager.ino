/*
	ESP-NOW Broadcast Slave
	Lucas Saavedra Vaz - 2024

	This sketch demonstrates how to receive broadcast messages from a master device using the ESP-NOW protocol.

	The master device will broadcast a message every 5 seconds to all devices within the network.

	The slave devices will receive the broadcasted messages. If they are not from a known master, they will be registered as a new master
	using a callback function.
*/

#include "ESP32_NOW.h"
#include "WiFi.h"
#include <esp_mac.h>
#include <vector>
#include <Servo.h>

/* Constants */

const int FRONT_SERVO_PIN = 23;
const int MOTOR_PIN = 22;

const int ESPNOW_WIFI_CHANNEL = 6;

/* Classes */

struct TransitStruct
{
	uint8_t throttle;
	uint8_t steer;
};

class ESP_NOW_Peer_Class : public ESP_NOW_Peer
{
public:
	// Constructor of the class
	ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk) : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

	// Destructor of the class
	~ESP_NOW_Peer_Class() {}

	// Function to register the master peer
	bool add_peer()
	{
		if (!add())
		{
			log_e("Failed to register the broadcast peer");
			return false;
		}
		return true;
	}

	// Function to print the received messages from the master
	void onReceive(const uint8_t *recieved, size_t len, bool broadcast)
	{
		if (sizeof(TransitStruct) != len)
		{
			Serial.printf("Wrong length packet recieved, len: %d, expected: %d", len, sizeof(TransitStruct));
			return;
		}
		// Change datatype without reallocating
		TransitStruct *data = (TransitStruct *)recieved;

		Serial.print("Steer: ");
		Serial.print(data->steer);
		Serial.print(", Throttle: ");
		Serial.println(data->throttle);

		lastMsg = *data;
	}
};

/* Global Variables */

// List of all the masters. It will be populated when a new master is registered
// Note: Using pointers instead of objects to prevent dangling pointers when the vector reallocates
std::vector<ESP_NOW_Peer_Class *> masters;

/* Callbacks */

// Callback called when an unknown peer sends a message,
// registers the peer as a master if the recieved messase is a broadcast
void register_new_master(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg)
{
	if (memcmp(info->des_addr, ESP_NOW.BROADCAST_ADDR, 6) == 0)
	{
		Serial.printf("Unknown peer " MACSTR " sent a broadcast message\n", MAC2STR(info->src_addr));
		Serial.println("Registering the peer as a master");

		ESP_NOW_Peer_Class *new_master = new ESP_NOW_Peer_Class(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, nullptr);
		if (!new_master->add_peer())
		{
			Serial.println("Failed to register the new master");
			delete new_master;
			return;
		}
		masters.push_back(new_master);
		Serial.printf("Successfully registered master " MACSTR " (total masters: %zu)\n", MAC2STR(new_master->addr()), masters.size());
	}
	else
	{
		// The slave will only receive broadcast messages
		log_v("Received a unicast message from " MACSTR, MAC2STR(info->src_addr));
		log_v("Igorning the message");
	}
}

/* Main */

Servo frontServo;
TransitStruct lastMsg;

void setup()
{
	Serial.begin(115200);

	frontServo.attach(FRONT_SERVO_PIN);
	pinMode(MOTOR_PIN, OUTPUT);

	// Initialize the Wi-Fi module
	WiFi.mode(WIFI_STA);
	WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
	while (!WiFi.STA.started())
	{
		delay(100);
	}

	Serial.println("  MAC Address: " + WiFi.macAddress());
	Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

	// Initialize the ESP-NOW protocol
	if (!ESP_NOW.begin())
	{
		Serial.println("Failed to initialize ESP-NOW");
		Serial.println("Reeboting in 5 seconds...");
		delay(5000);
		ESP.restart();
	}

	Serial.printf("ESP-NOW version: %d, max data length: %d\n", ESP_NOW.getVersion(), ESP_NOW.getMaxDataLen());

	// Register the new peer callback
	ESP_NOW.onNewPeer(register_new_master, nullptr);

	Serial.println("Setup complete. Waiting for a master to broadcast a message...");
}

void loop()
{
	// Print debug information every 10 seconds
	static unsigned long last_debug = 0;
	if (millis() - last_debug > 10000)
	{
		last_debug = millis();
		Serial.printf("Registered masters: %zu\n", masters.size());
		for (size_t i = 0; i < masters.size(); i++)
		{
			if (masters[i])
			{
				Serial.printf("  Master %zu: " MACSTR "\n", i, MAC2STR(masters[i]->addr()));
			}
		}
	}

	/* Main control code begin */

	frontServo.write(((int16_t)lastMsg.steer) * 180 / 255);
	// setMotorSpeed(lastMsg.throttle);

	float dist = afstandssensor.afstandCM();

	if (dist > 0)
	{
		if (dist < 50)
		{
			if (dist > 10)
			{
				float maxSpeed = dist * (255. / 25.);
				float speed = fminf(lastMsg.throttle, maxSpeed);
				speed = fminf(speed, 255); // max 255 before casting to uint8
				setMotorSpeed((uint8_t)speed);
			}
			else
			{
				setMotorSpeed(0);
			}
		}
		else
		{
			setMotorSpeed(lastMsg.throttle);
		}
	}

	delay(20);
}

void setMotorSpeed(uint8_t speed) {
	analogWrite(MOTOR_PIN, speed);
}