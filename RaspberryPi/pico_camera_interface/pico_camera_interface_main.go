package main

import (
	"fmt"
	"log"
	"os"

	"github.com/godbus/dbus/v5"
	"periph.io/x/conn/v3/i2c"
	"periph.io/x/conn/v3/i2c/i2creg"
	"periph.io/x/host/v3"
)

// Constants for D-Bus and I2C configurations
const (
	DBUS_PATH          = "/org/cacophony/thermalrecorder"
	DBUS_NAME          = "org.cacophony.thermalrecorder"
	I2C_DEVICE_ADDRESS = 0x15

	// Default X
	AXIS1_PIXEL_WIDTH = 159
	AXIS1_FOV         = 57 // From datasheet

	// Default Y
	AXIS2_PIXEL_WIDTH = 119
	AXIS2_FOV         = 57 * AXIS2_PIXEL_WIDTH / AXIS1_PIXEL_WIDTH // Scaled to fit pixel ratios

	INVERT_AXIS1 = false
	INVERT_AXIS2 = false
	SWITCH_AXES  = false
)

func initI2C() *i2c.Dev {
	// Initialize periph
	if _, err := host.Init(); err != nil {
		log.Fatal(err)
	}

	// Open the first available I2C bus
	bus, err := i2creg.Open("")
	if err != nil {
		log.Fatal(err)
	}
	defer bus.Close()

	// Specify the I2C device address
	device := &i2c.Dev{Addr: I2C_DEVICE_ADDRESS, Bus: bus}

	return device
}

func initDBUS() (*dbus.Conn, chan *dbus.Signal) {
	// Connect to the system bus
	conn, err := dbus.ConnectSystemBus()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
		os.Exit(1)
	}
	defer conn.Close()

	// Add match for the specific signal you want to receive
	if err := conn.AddMatchSignal(
		dbus.WithMatchObjectPath(DBUS_PATH),
		dbus.WithMatchInterface(DBUS_NAME),
	); err != nil {
		panic(err)
	}

	// Channel to receive signals
	c := make(chan *dbus.Signal, 10)
	conn.Signal(c)

	return conn, c
}

// Function to convert an array of pixel coordinates to degrees
func convertPixelsToDegree(coords []uint32) (int16, int16) {

	// Find center of target
	axis1_center := float32(coords[0]+coords[2]) / 2
	axis2_center := float32(coords[0]+coords[2]) / 2

	// Convert to angle from center
	axis1_angle := (axis1_center*AXIS1_FOV)/AXIS1_PIXEL_WIDTH - AXIS1_FOV/2
	axis2_angle := (axis2_center*AXIS2_FOV)/AXIS2_PIXEL_WIDTH - AXIS2_FOV/2

	if INVERT_AXIS1 {
		axis1_angle = -axis1_angle
	}
	if INVERT_AXIS2 {
		axis2_angle = -axis2_angle
	}

	// Multiplied by 10 for accuracy and converted to 16 bit integer for efficient i2c transmission
	if SWITCH_AXES {
		return int16(axis2_angle * 10), int16(axis1_angle * 10)
	} else {
		return int16(axis1_angle * 10), int16(axis2_angle * 10)
	}
}

func main() {

	device := initI2C()

	_, c := initDBUS()

	// Listening for signals
	for signal := range c {
		// Extract data
		what := signal.Body[0].(string)
		confidence := signal.Body[1].(int32)
		coords := signal.Body[2].([]uint32)
		tracking := signal.Body[3].(bool)

		// Process data
		x_angle, y_angle := convertPixelsToDegree(coords)

		// Print the content of the signal
		fmt.Printf("What = %s, Confidence = %d, Location = X: %d Y: %d, Tracking = %t\n",
			what, confidence, x_angle, y_angle, tracking)

		// Convert angles to bytes
		// Assuming the angles are within the range of int16
		x_angle_bytes := []byte{
			byte(x_angle >> 8),   // High byte of x_angle
			byte(x_angle & 0xFF), // Low byte of x_angle
		}
		y_angle_bytes := []byte{
			byte(y_angle >> 8),   // High byte of y_angle
			byte(y_angle & 0xFF), // Low byte of y_angle
		}

		// Combine bytes for x and y angles
		write_data := append([]byte{0x01}, x_angle_bytes...)
		write_data = append(write_data, y_angle_bytes...)

		// Write data to the I2C device
		_, err := device.Write(write_data)
		if err != nil {
			log.Println("Failed to write to device:", err)
		}
		fmt.Println("Sending bytes:", write_data)
	}
}
