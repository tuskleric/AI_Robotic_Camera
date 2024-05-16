package main

import (
	"fmt"
	"log"
	"math/rand"
	"time"

	"periph.io/x/conn/v3/i2c"
	"periph.io/x/conn/v3/i2c/i2creg"
	"periph.io/x/host/v3"
)

func main() {
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

	// Send random bytes to the I2C device
	rand.Seed(time.Now().UnixNano())
	writeData := []byte{0x01, byte(rand.Intn(256))}

	// Write data to the I2C device
	_, err = device.Write(writeData)
	if err != nil {
		log.Fatal("Failed to write to device:", err)
	}

	fmt.Println("Sent random bytes:", writeData)
}
