#include<stdio.h>
#include<stdlib.h>

#define uint64_t unsigned long long

#define MAX_ENCODED_LEN 2048

#define ENCODED_BITS_LEN 4

#define PACKET_LEN_VERSION 3
#define PACKET_LEN_TYPE 3
#define PACKET_LEN_LENGTH_TYPE 1
#define PACKET_LEN_LENGTH_BITS 15
#define PACKET_LEN_LENGTH_PACKETS 11
#define PACKET_LEN_GROUP 5

#define PACKET_TYPE_LITERAL 4

#define FOREACH_STATE(STATE) \
        STATE(P_VERSION) \
        STATE(P_TYPE) \
        STATE(P_LEN_TYPE) \
        STATE(P_LEN_BITS) \
        STATE(P_LEN_PACKETS) \
        STATE(P_LITERAL) \
        STATE(P_EXTRA) \

#define GENERATE_ENUM(ENUM) ENUM,

enum STATE_ENUM {
    FOREACH_STATE(GENERATE_ENUM)
};

typedef struct mstate {
  int state;
  uint64_t version, type, plen;
  uint64_t value, len;
} mstate_t;

uint64_t buff_add(char c, char buffer[], uint64_t blen) {
  c = c < 'A' ? c - '0' : c - 'A' + 10;
  uint64_t i = blen+ENCODED_BITS_LEN-1; 
  for(uint64_t j=0; j<ENCODED_BITS_LEN; j++) {
    buffer[i-j] = c & 1 ? 1 : 0;
    c >>= 1;
  }
  return ENCODED_BITS_LEN;
}

uint64_t buff_read(char *buffer, uint64_t *bpos, uint64_t len) {
  uint64_t i = *bpos;
  uint64_t v = 0;
  for(int j=0; j<len; j++) {
    uint64_t d = buffer[i+j];
    v += d << (len - j - 1);
  }
  *bpos += len;
  return v;
}

uint64_t decode(char buffer[], uint64_t *bidx, uint64_t blen, unsigned long long *vsum) {
  mstate_t m = {.state = P_VERSION, .version=-1, .type=-1, .plen=-1, .value = 0};
  uint64_t bpos = *bidx;
  while(m.state != P_EXTRA) {
    switch(m.state) {
      case P_VERSION:
        m.version = buff_read(buffer, &bpos, PACKET_LEN_VERSION);
        *vsum += m.version;
        m.state = P_TYPE;
        break;
      case P_TYPE:
        m.type = buff_read(buffer, &bpos, PACKET_LEN_TYPE);
        if(m.type == PACKET_TYPE_LITERAL) m.state = P_LITERAL;
        else m.state = P_LEN_TYPE;
        break;
      case P_LEN_TYPE:
        m.plen = buff_read(buffer, &bpos, PACKET_LEN_LENGTH_TYPE);
        if(!m.plen) m.state = P_LEN_BITS;
        else m.state = P_LEN_PACKETS;
        break;
      case P_LEN_BITS:
        m.len = buff_read(buffer, &bpos, PACKET_LEN_LENGTH_BITS);
        m.len += bpos;
        while(m.len > bpos)
          decode(buffer, &bpos, blen, vsum);
        m.state = P_EXTRA;
        break;
      case P_LEN_PACKETS:
        m.len = buff_read(buffer, &bpos, PACKET_LEN_LENGTH_PACKETS);
        for(int i=0; i<m.len; i++)
          decode(buffer, &bpos, blen, vsum);
        m.state = P_EXTRA;
        break;
      case P_LITERAL:
        if(!buff_read(buffer, &bpos, 1)) m.state = P_EXTRA;
        m.value <<= (PACKET_LEN_GROUP - 1);
        m.value += buff_read(buffer, &bpos, PACKET_LEN_GROUP - 1);
        break;
    }
  }
  *bidx = bpos;
  return m.value;
}

int main() {
  uint64_t bpos, blen;
  bpos = blen = 0;
  char buffer[MAX_ENCODED_LEN * ENCODED_BITS_LEN];
  unsigned long long vsum = 0;

  char c;
  while(scanf("%c", &c) > 0) {
    if((c < '0' || c > '9') && (c < 'A' || c > 'F')) break;
    blen += buff_add(c, buffer, blen);
  }
  
  decode(buffer, &bpos, blen, &vsum);
  // 873
  printf("Sum version number: %llu\n", vsum);
  
  return 0;
}