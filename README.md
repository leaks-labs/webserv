Config File Usage
=================

If no location is defined in a server block, a default location is created with the default settings.
In a server block, all location settings defined before creating a location override the default location's settings.
Empty lines are allowed in the config file.
If a setting is redefined, an override occurs.

Server Block settings
---------------------

| Keyword   | Possible values   | Default value | Comments  |
| :-------- | :---------------- | :------------ | :-------- |
| **`#`**               | **`ip adress`**   | **`0.0.0.0`** | create a new server block, must be followed by the server block's address |
| **`port`**            | **`port number`** | **`8080`**    | set the server block's port |
| **`server_names`**    | **`exemple.com`** |               | set a list of server names (separated with spaces) |

Location settings
-----------------

All location settings can be defined in Server Block level too.

| Keyword   | Possible values   | Default value | Comments  |
| :-------- | :---------------- | :------------ | :-------- |
| **`>`**               | **`/path/to/ressource`**                                          | `/`                   | create a new location that will match the url |
| **`>=`**              | **`/path/to/resource`**                                           | `/`                   | create a new absolute location that will match the url |
| **`root`**            | **`/path/`**                                                      | `/`                   | set the root of the location (root and alias overide each other) |
| **`alias`**           | **`/path/`**                                                      | `/`                   | set the alias of the location (root and alias overide each other) |
| **`default_file`**    | **`file.html`**                                                   | `index.html`          | when the url ends with `/`, set the default file to search inside the directory |
| **`redirect`**        | **`false`** OR **`http://exemple.org`** OR **`/other/location`**  | `false`               | set the redirection of a location                                                                                                                             
| **`errors`**          | **`error.html 404 500 502`**                                      |                       | set the error pages, first argument is the page and following argument are the errors separated with spaces (errors pages can be defined on multiple lines) |
| **`cgi`**             | **`none`** OR **`php-cgi`**                                       | **`php-cgi`**         | set the cgi |
| **`methods`**         | **`GET`** OR **`POST`** OR **`DELETE`** OR **`none`**             | **`GET POST DELETE`** | set the allowed methods, arguments is a list of method separated with spaces |
| **`bodymax`**         | **`0`** OR **`a positive number`**                                | **`1024`**            | set the max body size allowed send by the user to the server, 0 means infinite |
| **`listing`**         | **`true`** OR **`false`**                                         | **`false`**           | set the listing feature for directory targets |
| **`path_info`**       | **`none`** OR **`/path/to/upload/dir/`**                          | **`none`**            | set the destination folder for the upload files, the keyword none means the upload is not available |
