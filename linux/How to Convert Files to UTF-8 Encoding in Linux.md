
[How to Convert Files to UTF-8 Encoding in Linux](https://www.tecmint.com/convert-files-to-utf-8-encoding-in-linux/)

You can check the encoding of a file using the file command, by using the -i or --mime flag which enables printing of mime type string as in the examples below:

```sh
$ file -i Car.java
$ file -i CarDriver.jav
```

Convert Multiple Files to UTF-8 Encoding
Coming back to our main topic, to convert multiple or all files in a directory to UTF-8 encoding, you can write a small shell script called encoding.sh as follows:

```sh
#!/bin/bash
#enter input encoding here
FROM_ENCODING="value_here"
#output encoding(UTF-8)
TO_ENCODING="UTF-8"
#convert
CONVERT=" iconv  -f   $FROM_ENCODING  -t   $TO_ENCODING"
#loop to convert multiple files 
for  file  in  *.txt; do
     $CONVERT   "$file"   -o  "${file%.txt}.utf8.converted"
done
exit 0
```

