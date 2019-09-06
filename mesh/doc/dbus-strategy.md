# DBus Strategy

## Process

This is a asynchronous strategy:

- Request (call a DBus method), it will return immediately
- Get the result by listening some signals including error

## Error Handing

If there's any error occurs, a signal(executing) with a{sv} arguments of object(/org/embest/shell) will be emitted.

### Error Signal DBus Object

| Dest       | Interface                | Object Path      |
| ---------- | ------------------------ | ---------------- |
| org.embest | org.embest.MeshInterface | org/embest/shell |

### Dict

| Dict Key | Dict Variant Type | Description                                                       |
| -------- | ----------------- | ----------------------------------------------------------------- |
| "object" | o                 | Error from which DBus object                                      |
| "method" | s                 | Error from which DBus method(request)                             |
| "status" | i                 | Error code, 0 indicates success, values < 0 indicate error occurs |
| "error"  | s                 | Error message. When error code is 0, this field may be absence    |

## Menu

It's divided into differenct menu according to functions. You should call DBus method to enter the menu first when you want to execute its methods.

### Menu DBus Object

| Dest       | Interface                | Object Path      |
| ---------- | ------------------------ | ---------------- |
| org.embest | org.embest.MeshInterface | org/embest/shell |

### Menu DBus method

| Member    | Arguments In | Arguments Out | Description             |
| --------- | ------------ | ------------- | ----------------------- |
| menu_list | null         | as            | List all the menu name  |
| menu      | s            | null          | Enter the menu you want |
| back      | null         | null          | Return to main menu     |

### Menus

| Menu Name   | Description                   |
| ----------- | ----------------------------- |
| provisioner | This is the default main menu |
| config      | Node config                   |

