################################################################################
#                                 CONFIG                                       #
################################################################################

.DELETE_ON_ERROR:
.SILENT:

NAME:=  webserv

SRCS_DIR:= src
INC_DIRS= include
BUILD_DIR:= build

LDLIBS:=
LIB_DIRS=

CC:=	cc
CXX:=	c++

CXXFLAGS= -Wall -Wextra -Werror -std=c++98

CFLAGS=	-Wall -Wextra -Werror

CFLAGS+=	-Wconversion \
			-Wdouble-promotion \
			-Wfloat-equal \
			-Wformat=2 \
			-Winit-self \
			-fno-common \
			-Wshadow \
			-Wundef \
			-Wunused-macros \
			-Wwrite-strings \
			-Wmissing-prototypes \
			-Wmissing-declarations

#			-Wpedantic 
# 			-pedantic-errors
#			-Wcast-qual
#			-Wstrict-prototypes 
#			-Wpadded
#
#			gcc flags only
#			-Wformat-overflow
#			-Wformat-truncation
#			-Wlogical-op
#			-Wformat-signedness
#			-Wduplicated-cond
#			-Wduplicated-branches
#			-Walloc-zero

ADDITIONAL_CPPFLAGS=
ADDITIONAL_LDFLAGS=

################################################################################
#                                 PROGRAM'S SRCS                               #
################################################################################

EXT:=	.cpp

MAIN:=	main

SRCS_FILES:=	config_initialization/AcceptorRecords	\
				config_initialization/Location			\
				config_initialization/Server			\
				config_initialization/ServerList		\
				event_handlers/ConnectionAcceptor		\
				event_handlers/EventHandler				\
				event_handlers/StreamHandler			\
				handles/Acceptor						\
				handles/Stream							\
				InitiationDispatcher					\
				WebServ

################################################################################
#                                 SRC's FORMATING                              #
################################################################################

MAKEFILE_NAME:= ${lastword ${MAKEFILE_LIST}}

SRCS:=	${addprefix ${SRCS_DIR}/,${addsuffix ${EXT},${MAIN} ${SRCS_FILES}}}

OBJS_DIR:= ${BUILD_DIR}/objs
OBJS:=	${SRCS:${SRCS_DIR}/%${EXT}=${OBJS_DIR}/%.o}

DEPS_DIR:= ${BUILD_DIR}/deps
DEPS:=	${OBJS:${OBJS_DIR}/%.o=${DEPS_DIR}/%.d}

CPPFLAGS= ${addprefix -I,${INC_DIRS}} -MMD -MP -MF ${@:${OBJS_DIR}/%.o=${DEPS_DIR}/%.d}
CPPFLAGS+= ${ADDITIONAL_CPPFLAGS}

LDFLAGS= ${addprefix -L,${LIB_DIRS}}
LDFLAGS+= ${ADDITIONAL_LDFLAGS}

RM:=	rm -rf

COLOR_GREEN := \033[32m
COLOR_RESET := \033[0m

################################################################################
#                                 Makefile rules                               #
################################################################################

ifeq (${EXT}, .cpp)
	COMPILER:= ${CXX}
	FLAGS:= ${CXXFLAGS}
else
	COMPILER:= ${CC}
	FLAGS:= ${CFLAGS}
endif

all: ${NAME}

${NAME}: ${OBJS}
	${COMPILER} ${LDFLAGS} ${OBJS} ${LDLIBS} -o $@
	echo "${COLOR_GREEN}Compilation completed.${COLOR_RESET}"

${OBJS_DIR}/%.o: ${SRCS_DIR}/%${EXT} ${MAKEFILE_NAME}
	mkdir -p ${dir $@}
	mkdir -p ${dir ${@:${OBJS_DIR}/%.o=${DEPS_DIR}/%.d}}
	${COMPILER} ${CPPFLAGS} ${FLAGS} -c $< -o $@

clean:
	${RM} ${BUILD_DIR}
	echo "${COLOR_GREEN}Objects cleaned.${COLOR_RESET}"

fclean: clean
	${RM} ${NAME}
	echo "${COLOR_GREEN}Executables cleaned.${COLOR_RESET}"

re: fclean all

run: all
	echo "${COLOR_GREEN}Execuction of ./${NAME}:${COLOR_RESET}"
	./${NAME}

.PHONY:	all clean fclean re run

-include ${DEPS}
