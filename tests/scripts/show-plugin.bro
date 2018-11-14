# @TEST-EXEC: bro -NN Bro::AF_Packet |sed -e 's/version.*)/version)/g' > output
# @TEST-EXEC: btest-diff output
