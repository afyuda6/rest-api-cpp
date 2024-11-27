# rest-api-cpp
for learning purposes

## Create User
```c++
curl --location --request POST 'http://localhost:6000/users' \
--header 'Content-Type: application/x-www-form-urlencoded' \
--data-urlencode 'name=Yuda'
```

## Read Users
```c++
curl --location --request GET 'http://localhost:6000/users'
```

## Update User
```c++
curl --location --request PUT 'http://localhost:6000/users' \
--header 'Content-Type: application/x-www-form-urlencoded' \
--data-urlencode 'id=1' \
--data-urlencode 'name=Fajar'
```

## Delete User
```c++
curl --location --request DELETE 'http://localhost:6000/users' \
--header 'Content-Type: application/x-www-form-urlencoded' \
--data-urlencode 'id=1'
```