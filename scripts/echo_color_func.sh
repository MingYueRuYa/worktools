#/bin/bash

RED='\e[1;31m' # 红
RES='\e[0m'

GREEN='\033[32m'
GREEN_END='\033[0m'

function echo_red_color {
	echo -e "${RED}************$1************${RES}"
}

function echo_green_color {
	echo -e "${GREEN}************$1************${GREEN_END}"
}

