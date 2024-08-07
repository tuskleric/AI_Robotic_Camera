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
	AXIS1_FOV         = 100

	// Default Y
	AXIS2_PIXEL_WIDTH = 119
	AXIS2_FOV         = 50

	INVERT_X    = false
	INVERT_Y    = false
	SWITCH_AXES = false
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
func convertPixelsToDegree(coords []uint32) (float32, float32) {

	// Find center of target
	axis1_center := float32(coords[0]+coords[2]) / 2
	axis2_center := float32(coords[0]+coords[2]) / 2

	// Convert to angle from center
	axis1_angle := (axis1_center*AXIS1_FOV)/AXIS1_PIXEL_WIDTH - AXIS1_FOV/2
	axis2_angle := (axis2_center*AXIS2_FOV)/AXIS2_PIXEL_WIDTH - AXIS2_FOV/2

	if INVERT_X {
		axis1_angle = -axis1_angle
	}
	if INVERT_Y {
		axis2_angle = -axis2_angle
	}

	if SWITCH_AXES {
		return axis2_angle, axis1_angle
	} else {
		return axis1_angle, axis2_angle
	}
}

func main() {

	device := initI2C()

	conn, c := initDBUS()

	// Listening for signals
	for signal := range c {
		// Extract data
		what := signal.Body[0].(string)
		confidence := signal.Body[1].(int32)
		coords := signal.Body[2].([]uint32)
		tracking := signal.Body[3].(string)

		// Process data
		x_angle, y_angle := convertPixelsToDegree(coords)

		// Print the content of the signal
		fmt.Printf("What = %s, Confidence = %d, Location = X: %f Y: %f, Tracking = %s\n",
			what, confidence, x_angle, y_angle, tracking)

		writeData := []byte{0x01, byte(signal.Body[0].(int32))}

		fmt.Println("Sending bytes:", writeData)

		// Write data to the I2C device
		_, err := device.Write(writeData)
		if err != nil {
			log.Println("Failed to write to device:", err)
		}
	}
}
