# Open Food Facts API request in C

[![CircleCI](https://circleci.com/gh/angristan/openfoodfact-api-c.svg?style=svg)](https://circleci.com/gh/angristan/openfoodfact-api-c)


Simple program to get product information from the Open Food Facts API.

## Dependencies

- [curl](https://curl.haxx.se/libcurl/c/)
- [jansson](https://jansson.readthedocs.io/)

## Usage

```sh
$ make
$ ./get_off_product
usage: ./get_off_product <product id>
```

Example:

```sh
$ ./get_off_product 737628064502
Product name: Stir-Fry Rice Noodles
Product image: https://static.openfoodfacts.org/images/products/073/762/806/4502/front_en.6.400.jpg
```

### Docker

You can run the program in Docker with the provided Dockerfile:

```sh
$ docker build -t openfoodfact-api-c .
$ docker run --rm openfoodfact-api-c 737628064502
Product name: Stir-Fry Rice Noodles
Product image: https://static.openfoodfacts.org/images/products/073/762/806/4502/front_en.6.400.jpg
```

## License

MIT
