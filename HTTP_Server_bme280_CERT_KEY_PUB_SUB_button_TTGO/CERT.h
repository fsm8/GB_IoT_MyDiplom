
const char* root_ca = R"( 
-----BEGIN CERTIFICATE-----
MIIFGTCCAwGgAwIBAgIQJMM7ZIy2SYxCBgK7WcFwnjANBgkqhkiG9w0BAQ0FADAf
MR0wGwYDVQQDExRZYW5kZXhJbnRlcm5hbFJvb3RDQTAeFw0xMzAyMTExMzQxNDNa
Fw0zMzAyMTExMzUxNDJaMB8xHTAbBgNVBAMTFFlhbmRleEludGVybmFsUm9vdENB
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgb4xoQjBQ7oEFk8EHVGy
1pDEmPWw0Wgw5nX9RM7LL2xQWyUuEq+Lf9Dgh+O725aZ9+SO2oEs47DHHt81/fne
5N6xOftRrCpy8hGtUR/A3bvjnQgjs+zdXvcO9cTuuzzPTFSts/iZATZsAruiepMx
SGj9S1fGwvYws/yiXWNoNBz4Tu1Tlp0g+5fp/ADjnxc6DqNk6w01mJRDbx+6rlBO
aIH2tQmJXDVoFdrhmBK9qOfjxWlIYGy83TnrvdXwi5mKTMtpEREMgyNLX75UjpvO
NkZgBvEXPQq+g91wBGsWIE2sYlguXiBniQgAJOyRuSdTxcJoG8tZkLDPRi5RouWY
gxXr13edn1TRDGco2hkdtSUBlajBMSvAq+H0hkslzWD/R+BXkn9dh0/DFnxVt4XU
5JbFyd/sKV/rF4Vygfw9ssh1ZIWdqkfZ2QXOZ2gH4AEeoN/9vEfUPwqPVzL0XEZK
r4s2WjU9mE5tHrVsQOZ80wnvYHYi2JHbl0hr5ghs4RIyJwx6LEEnj2tzMFec4f7o
dQeSsZpgRJmpvpAfRTxhIRjZBrKxnMytedAkUPguBQwjVCn7+EaKiJfpu42JG8Mm
+/dHi+Q9Tc+0tX5pKOIpQMlMxMHw8MfPmUjC3AAd9lsmCtuybYoeN2IRdbzzchJ8
l1ZuoI3gH7pcIeElfVSqSBkCAwEAAaNRME8wCwYDVR0PBAQDAgGGMA8GA1UdEwEB
/wQFMAMBAf8wHQYDVR0OBBYEFKu5xf+h7+ZTHTM5IoTRdtQ3Ti1qMBAGCSsGAQQB
gjcVAQQDAgEAMA0GCSqGSIb3DQEBDQUAA4ICAQAVpyJ1qLjqRLC34F1UXkC3vxpO
nV6WgzpzA+DUNog4Y6RhTnh0Bsir+I+FTl0zFCm7JpT/3NP9VjfEitMkHehmHhQK
c7cIBZSF62K477OTvLz+9ku2O/bGTtYv9fAvR4BmzFfyPDoAKOjJSghD1p/7El+1
eSjvcUBzLnBUtxO/iYXRNo7B3+1qo4F5Hz7rPRLI0UWW/0UAfVCO2fFtyF6C1iEY
/q0Ldbf3YIaMkf2WgGhnX9yH/8OiIij2r0LVNHS811apyycjep8y/NkG4q1Z9jEi
VEX3P6NEL8dWtXQlvlNGMcfDT3lmB+tS32CPEUwce/Ble646rukbERRwFfxXojpf
C6ium+LtJc7qnK6ygnYF4D6mz4H+3WaxJd1S1hGQxOb/3WVw63tZFnN62F6/nc5g
6T44Yb7ND6y3nVcygLpbQsws6HsjX65CoSjrrPn0YhKxNBscF7M7tLTW/5LK9uhk
yjRCkJ0YagpeLxfV1l1ZJZaTPZvY9+ylHnWHhzlq0FzcrooSSsp4i44DB2K7O2ID
87leymZkKUY6PMDa4GkDJx0dG4UXDhRETMf+NkYgtLJ+UIzMNskwVDcxO4kVL+Hi
Pj78bnC5yCw8P5YylR45LdxLzLO68unoXOyFz1etGXzszw8lJI9LNubYxk77mK8H
LpuQKbSbIERsmR+QqQ==
-----END CERTIFICATE-----)";

const char* dev_cert_tx = R"(
-----BEGIN CERTIFICATE-----
MIIEpDCCAowCCQC9nEVMnLOv8zANBgkqhkiG9w0BAQsFADAUMRIwEAYDVQQDDAls
b2NhbGhvc3QwHhcNMjIwNDEzMTY1OTQ3WhcNMjMwNDEzMTY1OTQ3WjAUMRIwEAYD
VQQDDAlsb2NhbGhvc3QwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDa
bz80/gphtVmqKKp+x4AhxB1arUss8VyeHHst99/cFqFQD6rlOx9h//0RxGcEsTv/
dqP2pA4iSfhrRQJPqNCDfzRPfrnOECwrBHG2ysaIrOg5yydQdTMdUSR/8kh1J5aU
JRUttQ8HMcRF532Aheda9qHwvp2FMHGTcXvzhaWpFOhGwJ4O2rXwlroaEuTRVHHh
YUyuPQNyswYS26OKfK4zOU5IA3+tRDOL/hwCMHZMzszUyA19zfZPXn/uKLv3cyYG
1V5rRSR5/HiO/LIAGeIjfNHmNGw4ZyWLUAFMWM0YUURp9Tw/RHLxPn5e4gn/jT2m
vLd0IyUzpRTMs9hN6DCEWYssx2VpbSwESNh4CbPfTJneIomeYwxt6+hAL38HTV16
Cp00Ka7qgXBlGinaCpHKOyvda1wxcXDLRdKsht1EhaMbgBWbLEPCUM9CdgEnPYK+
jQ1aVoO5VPxwxG/cLIyugZWJImppcWE9zRKRReeTe984uVGBGrVrLlm9BeN++rwr
tPm62FPRZZhMDTH5Y17KPeTsKW1WxCGw566/RYudIMuJmUqRI14lVqJTPz6CPw6r
M7qI9m1qoIiV5dz2d72wcFtAoFj5qQuetAHTabCtQYx0ZDMngNivv7ehgUkb59RO
VX8EybX3Pus5cDtewzn7pKgtASW1BAXv4QDqG4/bLQIDAQABMA0GCSqGSIb3DQEB
CwUAA4ICAQCI6l4/VTuACgmEY4+YSTv31poNqMFVR1lJZPKz4WjHq93jaP3QvkmL
zic/AKSsZNbm6/hdcpPFzbWYg6x8o1FmzTRzuilU8wRO3PG0nyruhz20M28hkDS3
qa1dzQgUN03ms9qe0San9mBctwwoNlo9MLTXWtFNqOQakog2hOYhhrqFHPhhfoBh
6UJQJTisdJzuHxZ81HtJq3cJWQcPd6z57Al8mjahJj0AM7TMvoSXskWcbP4eFiPA
B7Uoi+EIZ/lXhF3YIrD0HkxT2ZsAfTXMmW+wVO7yndLgJ1RmFDzxGwPwOKN6U9Uc
VtuEcAO6O24AdfX8gQoi0oaw14mewkl9So67SCrlo13QyKQZnEDk9qHk+QaLvnA1
3SehVMWqgaxSbx4FxwDGEb2UY/HAeypfGDQtuB1/XUKkIprsweC6N/KSUadnpsAR
sBXQ4BzfltnpHnf2ZBU/4/6MHRwSjzU4GIXKibIXO/K5v9LYpTXpn61OwMoi1Mq3
WhopFAXOsDFjjg1j+TUHfjlnnkEnV7G1LkNzLz445xG2Cbq/7xBvOKLFcbYZ+dOC
XrGD+Upkj0RVRGod81PPeQdyXdzCVQdqnqZ+wwPfZIB/UbHY1174+I43jDChxFxM
5G3U+uabcKXoHCv2LXPl+j0L5i79HTtNL0BW8qC7LzXuBc6nNnb+fg==
-----END CERTIFICATE-----)";

const char* key_cert_tx = R"(
-----BEGIN PRIVATE KEY-----
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
UhWIBoEnX1QGuE7JaHsyErHLS79XUQ==
-----END PRIVATE KEY-----)";

const char* dev_cert_rx = R"(
-----BEGIN CERTIFICATE-----
MIIEpDCCAowCCQCo2QcjAgu1njANBgkqhkiG9w0BAQsFADAUMRIwEAYDVQQDDAls
b2NhbGhvc3QwHhcNMjIwNDEzMTY1MjE2WhcNMjMwNDEzMTY1MjE2WjAUMRIwEAYD
VQQDDAlsb2NhbGhvc3QwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCv
ZqCSvh5NVmZGcg/8gsC1+u6476hA4C3bsn3AxMcQa9W7bPKby3yw5AQ7Ru1498EF
hyVpJpk1CnbjzlurQQ3wpYLGldOQk9ksT2hAqtaJwgzccHz7KmzoyleEYhs7ILUK
vE99Ug7fzFVp0m9SOTxE82d8cJbUuMAU5F8TtfBPRzLxQjTTT20um02bkGjtwbOx
3dVSvz5KQAKjWQj4LznkM8/weQXK/bRSBFzzweT9INmxNLdt3aU2S/6Rfg3b7odx
jTN54IK13Atdrip+xg+McMeGbxflB4dN9FV2DGmy/IzAPnaTqsD+DCREndu6hrN8
JH/vOXTres4fHLNPURQ3usbc92YdYDfDNrjHIg5gRA44kwImOGfutZZ994ByOrpd
VG70CJhnvO72EGf1q8uv3EiMPsz4W1uPPX6Bz+wPA7gUG9RbdeDTB0mruOvGnqeE
KAldtXaaCvNfDAlI88K37cKwPRr/rbaSHfvR9MrDlbn0oGEixASvZgrH411bx1OT
rzhLClrHov+W7tqtY+IEO4vODhfQFb66LUcQ1IPUag8lvtEYgZmwYrCoXx2CJbfp
fzniGzm+zeyC98AT68R7ENezhwrEedzSTdYUaKeNZkPm9iLYMWZvyh+/dpTIaKbK
trTBxw8Re/HAs2ORUEXNym9wvsoUJOlD9NqSCdiQMwIDAQABMA0GCSqGSIb3DQEB
CwUAA4ICAQBvBUhSxV1V+4YoZPpWpAb6YOH5KFcwP1loDm0If8/1jV4oqlt1R93i
EgowQjznQQCFsG1BArIei5VYkpY8BW89KVmit3qLfHxWPG6i775xLTobl2JRANmX
aMt5ZB1DqNwmEqDVWj1b3dOrsCx/RKMyg4Gf0tuYoRqBsaGadmDLjOazY0aeEkld
0WXV4h6YlyjLfX9Mw6dusJjbSL0z6wNqDe3+JXv60geyU2a8lZhuxiduPanHIkDr
Wep6NHDwE0nT3GGycUTpyAL0N4CvGfPWBiHnrt63OHt4hsoo0bE6Y1CAFjk7wi3z
EEP/3fnzO+tpjaq5uUCZOF2KBlLBozsUqtO31BdKYcLGXZdUfBcWQdQsW6b+u2Lp
b1lrjPkNVICgv+FTfh1I7abpmn18PdlA13s4luz5D2ta//RMEKKObC6WKAeKMtpt
lmOV3gdfzZxSxXYLcdb2kadFmd88ovsWWrm4urubZVvbD2snBldp/Ee/W1iwbB8t
oLNvG8RC4FcHJKvTS6HNBTB4xchJkKrMWGi1m+fLe5YISmKcA8x8SXLaDQ0ltDC/
2nnwKc/4ANA+Nh0I74rjHlBsseneCkTtYF366lu1uV3WGCKaEpPOkNPm1WSPLgp5
3lyt5oVtGesyzwZ2B80EIv0InbnnIAe8PkYzargY/1FmJLYCvHgt0g==
-----END CERTIFICATE-----)";

const char* key_cert_rx = R"(
-----BEGIN PRIVATE KEY-----
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
RF5rEBcYRLIQZ/fKYgtO3sL450SHL+o=
-----END PRIVATE KEY-----)";
