/*
 * rdata.c
 *
 * rdata implementation
 *
 * a Net::DNS like library for C
 *
 * (c) NLnet Labs, 2004
 *
 * See the file LICENSE for the license
 */

#include <config.h>

#include <ldns/rdata.h>

#include "util.h"
#include "error.h"

/* Access functions 
 * do this as functions to get type checking
 */

/* read */
uint16_t
_ldns_rd_field_size(t_rdata_field *rd)
{
	return rd->_size;
}

ldns_rdata_field_type
_ldns_rd_field_type(t_rdata_field *rd)
{
	return rd->_type;
}

uint8_t *
_ldns_rd_field_data(t_rdata_field *rd)
{
	return rd->_data;
}

/* write */
void
_ldns_rd_field_set_size(t_rdata_field *rd, uint16_t s)
{
	rd->_size = s;
}

void
_ldns_rd_field_set_type(t_rdata_field *rd, ldns_rdata_field_type t)
{
	rd->_type = t;
}

void
_ldns_rd_field_set_data(t_rdata_field *rd, uint8_t *d)
{
	/* only copy the pointer */
	rd->_data = d;
}

/**
 * Allocate a new t_rdata_field structure 
 * and return it
 */
t_rdata_field *
_ldns_rd_field_new(uint16_t s, ldns_rdata_field_type t, uint8_t *d)
{
	t_rdata_field *rd;
	rd = MALLOC(t_rdata_field);
	if (!rd) {
		return NULL;
	}

	_ldns_rd_field_set_size(rd, s);
	_ldns_rd_field_set_type(rd, t);
	_ldns_rd_field_set_data(rd, d);

	return rd;
}

/**
 * Allocate a new t_rdata_field from
 * a NULL terminated string
 * and return it
 */
t_rdata_field *
_ldns_rd_field_new_frm_string(ldns_rdata_field_type t, char *s)
{
	return NULL;
}

void 
_ldns_rd_field_destroy(t_rdata_field *rd)
{
	rd = NULL; /* kuch */
	/* empty */
}

/**
 * remove \DDD, \[space] and other escapes from the input
 * See RFC 1035, section 5.1
 * Return the lenght of the string or a negative error
 * code
 */
ldns_t_status
_ldns_octet(char *word)
{
    char *s; char *p;
    unsigned int length = 0;

    for (s = p = word; *s != '\0'; s++,p++) {
        switch (*s) {
            case '.':
                if (s[1] == '.') {
                    fprintf(stderr,"Empty label");
		    return EEMPTY_LABEL;
                }
                *p = *s;
                length++;
                break;
            case '\\':
                if ('0' <= s[1] && s[1] <= '9' &&
                    '0' <= s[2] && s[2] <= '9' &&
                    '0' <= s[3] && s[3] <= '9')
                {
                    /* \DDD seen */
                    int val = ((s[1] - '0') * 100 +
                           (s[2] - '0') * 10 + (s[3] - '0'));

                    if (0 <= val && val <= 255) {
                        /* this also handles \0 */
                        s += 3;
                        *p = val;
                        length++;
                    } else {
                        fprintf(stderr,"ASCII \\DDD overflow");
                    }
                } else {
                    /* an espaced character, like \<space> ? 
                    * remove the '\' keep the rest */
                    *p = *++s;
                    length++;
                }
                break;
            case '\"':
                /* non quoted " Is either first or the last character in
                 * the string */

                *p = *++s; /* skip it */
                length++;
		/* I'm not sure if this is needed in libdns... MG */
                if ( *s == '\0' ) {
                    /* ok, it was the last one */
                    *p  = '\0'; return length;
                }
                break;
            default:
                *p = *s;
                length++;
                break;
        }
    }
    *p = '\0';
    return length;
}
