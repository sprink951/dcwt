TARGET = dcwt_server
CFLAGS  += -MD -Werror

CFLAGS  += -I. -I./dcwtnet 
				

LDFLAGS += \
          -pthread -lm  
#######################################################################################################

SOURCE = \
	dcwt_socket.c\
	dcwt_host.c \
	dcwt_service.c\
	dcwt_protocol.c \
	dcwt_main.c
#######################################################################################################
.PHONY: clean install
#######################################################################################################

OBJS = $(SOURCE:%.c=obj.$(TARGET)/%.o)
DEPS = $(SOURCE:%.c=obj.$(TARGET)/%.d)

default:
	make $(TARGET)

$(TARGET):$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
#	$(STRIP) $@

obj.$(TARGET)/%.o:%.c
	@mkdir -p obj.$(TARGET)/dcwtnet
	$(CC) $(CFLAGS) -c -o $@ $<

#######################################################################################################
clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS) obj.$(TARGET)

install:
	cp -a $(TARGET) $(EBI_INSTALL_DIR)/sbin
#	cp -a ./lib/iconv/lib/libiconv.so* $(EBI_INSTALL_DIR)/lib
#	make -C ./locate_debug install
#	make -C ./lib/mempool/  install
#	cp -a mac2org.log $(EBI_INSTALL_DIR)/sbin

