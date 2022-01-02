package server

import (
	"bufio"
	"fmt"
	"net"
	"strconv"
	"strings"
)

var system_prefix string = "server -> "

func Start() {
	Init()
}

func Init() {

	const version string = "v0.0.1"
	const connectionPort int = 8000

	fmt.Println("=+=------------------=+=")
	fmt.Println("PasswordManagment-Server-" + version)
	fmt.Println("=+=------------------=+=")
	fmt.Println("")
	fmt.Println("")

	server, _ := net.Listen("tcp", ":"+strconv.Itoa(connectionPort))

	fmt.Println("listening to sockets . . .")
	for {
		socket, _ := server.Accept()

		message, _ := bufio.NewReader(socket).ReadString('\n')

		//fmt.Println("Message Received encrypted:" + string(message) + ", size: " + strconv.Itoa(len(string(message))))

		// decrypt message encryption
		//decryptedMsg := XOR(string(message), "KCQ")

		fmt.Println(system_prefix+"Message Received: ", message)

		onPacketReceive(message, socket)
	}
}

func onPacketReceive(msg string, connection net.Conn) {

	// obviusly use SSL in the future, too lazy to set it up rn
	// we are using the md5 hash of our keypass since it always has to
	// be 32 bits
	//communication_encryption_key := GenerateMD5("mysupersecretkeypass")

	// decrypt msg here
	//msg = Decrypt(msg, communication_encryption_key)

	args := strings.Split(msg, "|")

	if len(args) < 2 {
		connection.Write([]byte("incorrect args"))
		return
	}

	action := args[0]
	authKey := args[1]

	authKeyHash := hashAuthKey(authKey)

	fmt.Println(system_prefix + "processing message . . .")

	if len(args) == 2 {

		if action == "vaultRequest" {

			// can't return a vault that doesn't exist
			if !Exists(authKeyHash) {
				fmt.Println(system_prefix + "vault " + authKeyHash + " not found")
				connection.Write([]byte("vault not found"))
				return
			}

			//fmt.Println("vault found!")

			// read vault from database
			vault := ScanFile(authKeyHash)

			// send vault to client
			fmt.Println(system_prefix + "sending vault to " + authKeyHash)
			connection.Write([]byte(vault))

		} else if action == "deleteVault" {
			// can't delete a vault that doesn't exist
			if !Exists(authKeyHash) {
				fmt.Println(system_prefix + "vault " + authKeyHash + " not found")
				connection.Write([]byte("vault not found"))
				return
			}

			DeleteFile(authKeyHash)

			// send vault to client
			fmt.Println(system_prefix + "deleting vault " + authKeyHash)
			connection.Write([]byte("vault deleted"))
		}

	} else if len(args) == 3 {

		data := args[2]

		if action == "createVault" {

			/* If there is already a vault using these credentials */
			if Exists(authKeyHash) {
				connection.Write([]byte("you already have a vault"))
				return
			}

			// writing new data to database
			WriteFile(authKeyHash, data)

			fmt.Println(system_prefix + "vault created for user " + authKeyHash)
			connection.Write([]byte("new vault created!"))
		} else if action == "updateVault" {

			// can't update a vault that doesn't exist
			if !Exists(authKeyHash) {
				connection.Write([]byte("vault not found"))
				return
			}

			// delete old vault
			DeleteFile(authKeyHash)

			// create new vault
			WriteFile(authKeyHash, data)

			fmt.Println(system_prefix + "vault updated for user " + authKeyHash)
			connection.Write([]byte("vault updated!"))
		}

	}
}

// hashing and salting to safely store as the vault auth key
func hashAuthKey(plainKey string) string {
	const static_salt_back = "fdjgiernorwssdklsvmlmawknqhfudsuf"
	const static_salt_front = "bjoikpdfosivhuywejraerfdshytikuyhfdgf"
	hashedKey := plainKey
	for i := 0; i < 300; i++ {
		hashedKey = GenerateSha512(static_salt_back + hashedKey + static_salt_front)
	}
	return hashedKey
}
