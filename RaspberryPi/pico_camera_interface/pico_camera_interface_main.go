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

func main() {
	conn, err := dbus.ConnectSystemBus()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
		os.Exit(1)
	}
	defer conn.Close()

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
	device := &i2c.Dev{Addr: 0x15, Bus: bus}

	// Add match for the specific signal you want to receive
	if err := conn.AddMatchSignal(
		dbus.WithMatchObjectPath("/org/cacophony/thermalrecorder"), //"/org/freedesktop/DBus"),
		dbus.WithMatchInterface("org.cacophony.thermalrecorder"),   //"org.freedesktop.DBus"),
	); err != nil {
		panic(err)
	}

	// Channel to receive signals
	c := make(chan *dbus.Signal, 10)
	conn.Signal(c)

	// Listening for signals
	for signal := range c {
		// Print the content of the signal
		fmt.Printf("Received signal: Path=%s, Sender=%s, Body=%v\n",
			signal.Path, signal.Sender, signal.Body)

		writeData := []byte{0x01, byte(signal.Body[0].(int32))}

		fmt.Println("Sending bytes:", writeData)

		// Write data to the I2C device
		_, err = device.Write(writeData)
		if err != nil {
			log.Println("Failed to write to device:", err)
		}
	}
}
