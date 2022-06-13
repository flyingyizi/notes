
[格式化](https://magiclen.org/rust-formatted-string/)

文本格式化的范例如下：
```rust
format!("Hello");                                     // => "Hello"
format!("Hello, {}!", "world");                       // => "Hello, world!"
format!("The number is {}", 1);                       // => "The number is 1"
format!("{:?}", (3, 4));                              // => "(3, 4)"
format!("{1:?} {0:?}", (3, 4), (5, 6));               // => "(5, 6) (3, 4)"
format!("{value}", value = 4);                        // => "4"
format!("{value1} {value2}", value1 = 4, value2 = 5); // => "4 5"
format!("{name} {}", 1, name = "MagicLen");           // => "MagicLen 1"
format!("{} {}", 1, 2);                               // => "1 2"
format!("{0} {1}", 1, 2);                             // => "1 2"
format!("{1} {0}", 1, 2);                             // => "2 1"
format!("{1} {} {2} {} {0} {}", 1, 2, 3);             // => "2 1 3 2 1 3"
format!("{:4}", 42);                                  // => "  42" with leading spaces
format!("{:<4}", 42);                                 // => "42  " with trailing spaces
format!("{:04}", 42);                                 // => "0042" with leading zeros
format!("{:0<4}", 42);                                // => "4200" with trailing zeros
format!("{:+}", 42);                                  // => "+42"
format!("{:b}", 42);                                  // => "101010"
format!("{:#b}", 42);                                 // => "0b101010"
format!("{:.*}", 2, 1.2345);                          // => "1.23"
format!("{:.*}", 3, 1.2345);                          // => "1.234"
format!("{:.*}", 3, 1.2335);                          // => "1.234"
format!("{:.3}", 1.2335);                             // => "1.234"
format!("{:7.3}", 1.2335);                            // => "  1.234" with leading spaces
format!("{:<7.3}", 1.2335);                           // => "1.234  " with trailing spaces
format!("{:07.3}", 1.2335);                           // => "001.234" with leading zeros
format!("{:0>7.3}", 1.2335);                          // => "001.234" with leading zeros
format!("{:<07.3}", 1.2335);                          // => "001.234" with leading zeros
format!("{:0<7.3}", 1.2335);                          // => "1.23400" with trailing zeros
format!("{:07.3}", -1.2335);                          // => "-01.234" with leading spaces after the sign character
format!("{:0>7.3}", -1.2335);                         // => "0-1.234" with leading spaces before the sign character
format!("{:<07.3}", -1.2335);                         // => "-01.234" with leading zeros after the sign character
format!("{:0<7.3}", -1.2335);                         // => "-1.2340" with trailing zeros
format!("{:@>5}", "Hi");                              // => "@@@Hi" with leading '@'s
format!("{:@<5}", "Hi");                              // => "Hi@@@" with trailing '@'s
format!("{:@^5}", "Hi");                              // => "@Hi@@" with leading and trailing '@'s (center alignment)
format!("{3:.2$} {1:.0$}", 2, 1.2345, 1, 1.2345);     // => "1.2 1.23"
format!("{second:.second_decimal$} {first:.first_decimal$}", first_decimal = 2, first = 1.2345, second_decimal = 1, second = 1.2345); // => "1.2 1.23"
```

