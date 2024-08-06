// package main

// import (
// 	"fmt"
// 	"log"
// 	"math/rand"
// 	"time"

// 	"periph.io/x/conn/v3/i2c"
// 	"periph.io/x/conn/v3/i2c/i2creg"
// 	"periph.io/x/host/v3"
// 	"github.com/godbus/dbus/v5"
// )

package main

import (
	"fmt"
	"os"

	"github.com/godbus/dbus/v5"
)

// func main() {
// 	conn, err := dbus.ConnectSystemBus()
// 	if err != nil {
// 		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
// 		os.Exit(1)
// 	}
// 	defer conn.Close()

// 	reader := bufio.NewReader(os.Stdin)
// 	fmt.Print("Enter an integer: ")
// 	input, _ := reader.ReadString('\n')
// 	input = strings.TrimSpace(input)
// 	number, err := strconv.Atoi(input)
// 	if err != nil {
// 		fmt.Println("Error: Invalid input. Please enter a valid integer.")
// 		return
// 	}

// 	signalData := number

// 	// Emitting a signal
// 	err = conn.Emit(dbus.ObjectPath("/org/cacophony/DBus/test"), "org.cacophony.DBus.test.NameLost", signalData)
// 	if err != nil {
// 		fmt.Fprintln(os.Stderr, "Failed to emit signal:", err)
// 		os.Exit(1)
// 	}

// 	fmt.Println("Signal emitted successfully!")

// 	// Sleep to allow time for the signal to be processed (optional)
// 	time.Sleep(1 * time.Second)
// }

func main() {
	conn, err := dbus.ConnectSystemBus()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
		os.Exit(1)
	}
	defer conn.Close()

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
	}
}

// func main() {
// 	conn, err := dbus.ConnectSystemBus()
// 	if err != nil {
// 		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
// 		os.Exit(1)
// 	}
// 	defer conn.Close()

// 	// Add match for the specific signal you want to receive
// 	if err := conn.AddMatchSignal(); err != nil {
// 		panic(err)
// 	}

// 	// Channel to receive signals
// 	c := make(chan *dbus.Signal, 10)
// 	conn.Signal(c)

// 	// Listening for signals
// 	for signal := range c {
// 		// Print the content of the signal
// 		if signal.Path != "/org/freedesktop/DBus" {
// 			fmt.Printf("Received signal from interface: %s, path: %s\n", signal.Path, signal.Name)
// 		}
// 	}
// }

// func main() {
// 	conn, err := dbus.ConnectSystemBus()
// 	if err != nil {
// 		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
// 		os.Exit(1)
// 	}
// 	defer conn.Close()

// 	// Initialize periph
// 	if _, err := host.Init(); err != nil {
// 		log.Fatal(err)
// 	}

// 	// Open the first available I2C bus
// 	bus, err := i2creg.Open("")
// 	if err != nil {
// 		log.Fatal(err)
// 	}
// 	defer bus.Close()

// 	// Specify the I2C device address
// 	device := &i2c.Dev{Addr: 0x15, Bus: bus}

// 	// Add match for the specific signal you want to receive
// 	if err := conn.AddMatchSignal(
// 		dbus.WithMatchObjectPath("/org/cacophony/DBus/test"), //"/org/freedesktop/DBus"),
// 		dbus.WithMatchInterface("org.cacophony.DBus.test"),   //"org.freedesktop.DBus"),
// 	); err != nil {
// 		panic(err)
// 	}

// 	// Channel to receive signals
// 	c := make(chan *dbus.Signal, 10)
// 	conn.Signal(c)

// 	// Listening for signals
// 	for signal := range c {
// 		// Print the content of the signal
// 		fmt.Printf("Received signal: Path=%s, Sender=%s, Body=%v\n",
// 			signal.Path, signal.Sender, signal.Body)

// 		writeData := []byte{0x01, byte(signal.Body[0].(int32))}

// 		fmt.Println("Sending bytes:", writeData)

// 		// Write data to the I2C device
// 		_, err = device.Write(writeData)
// 		if err != nil {
// 			log.Println("Failed to write to device:", err)
// 		}
// 	}
// }

// func main() {
// 	// Initialize periph
// 	if _, err := host.Init(); err != nil {
// 		log.Fatal(err)
// 	}

// 	// Open the first available I2C bus
// 	bus, err := i2creg.Open("")
// 	if err != nil {
// 		log.Fatal(err)
// 	}
// 	defer bus.Close()

// 	// Specify the I2C device address
// 	device := &i2c.Dev{Addr: 0x15, Bus: bus}

// 	// Send random bytes to the I2C device
// 	rand.Seed(time.Now().UnixNano())
// 	writeData := []byte{0x01, byte(rand.Intn(256))}

// 	// Write data to the I2C device
// 	_, err = device.Write(writeData)
// 	if err != nil {
// 		log.Fatal("Failed to write to device:", err)
// 	}

// 	fmt.Println("Sent byte:", writeData)
// }
