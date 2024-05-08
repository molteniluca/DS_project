network NetworkRing
{
    @display("bgb=800,800");
    submodules:
        c0: ClientRing;
        c1: ClientRing;
        c2: ClientRing;
        c3: ClientRing;
        c4: ClientRing;

    connections allowunconnected:
        c0.out_right --> c1.in_left;
        c1.out_right --> c2.in_left;
        c2.out_right --> c3.in_left;
        c3.out_right --> c4.in_left;
        c4.out_right --> c0.in_left;

        c0.out_left --> c4.in_right;
        c4.out_left --> c3.in_right;
        c3.out_left --> c2.in_right;
        c2.out_left --> c1.in_right;
        c1.out_left --> c0.in_right;
}