/*
 *   Proposed Digital Signature Standard (DSS)
 *
 *   Elliptic Curve Variation GF(2^m) - See Dr. Dobbs Journal April 1997
 *
 *   This program verifies the signature given to a <file> in
 *   <file>.ecs generated by program ecsign
 * 
 *   The curve is y^2+xy = x^3+Ax^2+B over GF(2^m) using a trinomial or 
 *   pentanomial basis (t^m+t^a+1 or t^m+t^a+t^b+t^c+1). These parameters
 *   can be generated using the findbase.cpp example program, or taken from tables
 *   provided, for example in IEEE-P1363 Annex A
 *
 *   The file common2.ecs is presumed to exist and contain 
 *   {m,A,B,q,x,y,a,b,c,q} where A and B are parameters of the equation 
 *   above, (x,y) is an initial point on the curve, {m,a,b,c} are the field 
 *   parameters, (b is zero for a trinomial) and q is the order of the 
 *   (x,y) point, itself a large prime. The number of points on the curve is 
 *   cf.q where cf is the "co-factor", normally 2 or 4.
 * 
 *   Requires: big.cpp ec2.cpp
 *
 *   Copyright (c) 2000-2003 Shamus Software Ltd.
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include "ec2.h"

using namespace std;

Miracl precision(200,256);

void strip(char *name)
{ /* strip off filename extension */
    int i;
    for (i=0;name[i]!='\0';i++)
    {
        if (name[i]!='.') continue;
        name[i]='\0';
        break;
    }
}

static Big hash(ifstream &fp)
{ /* compute hash function */
    char ch,s[20];
    Big h;
    sha sh;
    shs_init(&sh);
    forever 
    { /* read in bytes from message file */
        fp.get(ch);
        if (fp.eof()) break;
        shs_process(&sh,ch);
    }
    shs_hash(&sh,s);
    h=from_binary(20,s);
    return h;
}

int main()
{
    ifstream common("common2.ecs");    /* construct file I/O streams */
    ifstream public_key("public.ecs");
    ifstream message;
    ifstream signature;
    EC2 G,Pub;
    int ep,m,a,b,c;
    Big a2,a6,q,x,y,v,u1,u2,r,s,h;
    char ifname[50],ofname[50];
    miracl *mip=&precision;

/* get public data */

    common >> m;
    mip->IOBASE=16;
    common >> a2 >> a6 >> q >> x >> y;
    mip->IOBASE=10;
    common >> a >> b >> c;

    ecurve2(m,a,b,c,a2,a6,FALSE,MR_PROJECTIVE);
    G=EC2(x,y);
/* get public key of signer */
    public_key >> ep >> x;
    Pub=EC2(x,ep);         // decompress
/* get message */
    cout << "signed file = " ;
    cin.sync();
    cin.getline(ifname,13);
    strcpy(ofname,ifname);
    strip(ofname);
    strcat(ofname,".ecs");
    message.open(ifname,ios::binary|ios::in); 
    if (!message)
    { /* no message */
        cout << "Unable to open file " << ifname << "\n";
        return 0;
    }
    h=hash(message);

    mip->IOBASE=10;
    signature.open(ofname,ios::in);
    if (!signature)
    { /* no signature */
        cout << "signature file " << ofname << " does not exist\n";
        return 0;
    }
    signature >> r >> s;
    if (r>=q || s>=q)
    {
        cout << "Signature is NOT verified\n";
        return 0;
    }
    s=inverse(s,q);
    u1=(h*s)%q;
    u2=(r*s)%q;

    G=mul(u2,Pub,u1,G);
    G.get(v);
    v%=q;
    if (v==r) cout << "Signature is verified\n";
    else      cout << "Signature is NOT verified\n";
    return 0;
}

