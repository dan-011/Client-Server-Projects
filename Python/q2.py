import subprocess

print("All set")
# 172.16.51. 60 + group number (2 to 15)
counter = 0
unused_ranges = []
active_ranges = []
for j in range(48, 52):
  for i in range(0, 256):
    print("172.16." + str(j) + "." + str(i))
    try:
      _output = subprocess.check_output([ "ping", "-W", "0.01", "172.16." + str(j) + "." + str(i), "-c", "1"])
      active_ranges.append("172.16." + str(j) + "." + str(i))
    except subprocess.CalledProcessError:
      unused_ranges.append("172.16." + str(j) + "." + str(i))
      continue
    output = _output.decode("utf-8")
    print(output)
    if "Destination Host Unreachable" not in output.strip():
      counter += 1
print("\n")
print("\n")
print("\n")
print(len(active_ranges))
print("Pings Received: " + str(counter) + "/1024")
print("\n")
print("Unused Ranges:")
for ip in unused_ranges:
  print(ip)
print("\n")
print("Number of Unused Ranges: " + str(len(unused_ranges)) + "/1024")
f = open("active_ranges", 'w+')
for _range in active_ranges:
  f.write(_range+'\n')
f.close()