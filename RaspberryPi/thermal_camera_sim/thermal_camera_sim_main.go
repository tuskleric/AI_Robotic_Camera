package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/godbus/dbus/v5"
)

func main() {
	conn, err := dbus.ConnectSystemBus()
	if err != nil {
		fmt.Fprintln(os.Stderr, "Failed to connect to session bus:", err)
		os.Exit(1)
	}
	defer conn.Close()

	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter an integer: ")
	input, _ := reader.ReadString('\n')
	input = strings.TrimSpace(input)
	number, err := strconv.Atoi(input)
	if err != nil {
		fmt.Println("Error: Invalid input. Please enter a valid integer.")
		return
	}

	signalData := number

	// Emitting a signal
	err = conn.Emit(dbus.ObjectPath("/org/cacophony/DBus/test"), "org.cacophony.DBus.test.NameLost", signalData)
	if err != nil {
		fmt.Fprintln(os.Stderr, "Failed to emit signal:", err)
		os.Exit(1)
	}

	fmt.Println("Signal emitted successfully!")

	// Sleep to allow time for the signal to be processed (optional)
	time.Sleep(1 * time.Second)
}
