package server

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/md5"
	"crypto/rand"
	"crypto/sha256"
	"crypto/sha512"
	"encoding/hex"
	"io"
)

func Encrypt(str string, passphrase string) []byte {
	var data []byte = []byte(str)
	block, _ := aes.NewCipher([]byte(GenerateMD5(passphrase)))
	gcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}
	nonce := make([]byte, gcm.NonceSize())
	if _, err = io.ReadFull(rand.Reader, nonce); err != nil {
		panic(err.Error())
	}
	ciphertext := gcm.Seal(nonce, nonce, data, nil)
	return ciphertext
}

func Decrypt(str string, passphrase string) string {
	var data []byte = []byte(str)
	key := []byte(GenerateMD5(passphrase))
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err.Error())
	}
	gcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}
	nonceSize := gcm.NonceSize()
	nonce, ciphertext := data[:nonceSize], data[nonceSize:]
	plaintext, err := gcm.Open(nil, nonce, ciphertext, nil)
	if err != nil {
		panic(err.Error())
	}
	return string(plaintext)
}

func GenerateSha256(text string) string {
	hasher := sha256.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

func GenerateSha512(text string) string {
	hasher := sha512.New()

	hasher.Write([]byte(text))

	return hex.EncodeToString(hasher.Sum(nil))
}

func GenerateMD5(text string) string {
	hash := md5.Sum([]byte(text))
	return hex.EncodeToString(hash[:])
}
