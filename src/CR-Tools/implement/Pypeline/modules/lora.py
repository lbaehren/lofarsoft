"""LORA
"""

def loraTimestampToBlocknumber(lora_seconds, lora_nanoseconds, starttime, samplenumber, clockoffset = 1e4, blocksize = 2**16):
    """Calculates block number corresponding to LORA timestamp.
    The LORA timestamp is given in *lora_seconds* and *lora_nanoseconds*.
    From the LOFAR TBB event you need the *starttime* in seconds, the
    corresponding *samplenumber*, the *clockoffset* between LORA and LOFAR.
    Finally you need the *blocksize* used for reading the LOFAR data.
    """

    lora_samplenumber = (lora_nanoseconds - clockoffset) / 5

    value = (lora_samplenumber - samplenumber) + 2e8 * (lora_seconds - starttime)

    if value < 0:
        raise ValueError("Event not in file.")

    return int(value / blocksize)

