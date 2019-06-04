# David LaMartina
# lamartid@oregonstate.edu
# Program 4: OTP Encryption & Decryption
# CS344 Spr2019
# Due June 9, 2019

CC		= gcc
CFLAGS	= -g

all: otp_enc otp_enc_d otp_dec otp_dec_d keygen

otp_enc: otp_enc.c utilities.c textHandle.c
	$(CC) $(CFLAGS) -o otp_enc otp_enc.c utilities.c textHandle.c

otp_enc_d: otp_enc_d.c utilities.c
	$(CC) $(CFLAGS) -o otp_enc_d otp_enc_d.c utilities.c

otp_dec:
	$(CC) $(CFLAGS) -o otp_dec otp_dec.c

otp_dec_d:
	$(CC) $(CFLAGS) -o otp_dec_d otp_dec_d.c

keygen:
	$(CC) $(CFLAGS) -o keygen keygen.c

clean:
	rm -rf otp_enc otp_enc_d otp_dec otp_dec_d keygen *.o*
