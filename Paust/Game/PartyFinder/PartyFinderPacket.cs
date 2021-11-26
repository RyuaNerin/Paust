using System.Linq;
using System.Runtime.InteropServices;

namespace Paust.PartyFinder
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct PartyFinderPacket
    {
        /// <summary>
        /// Gets the size of this packet.
        /// </summary>
        internal static int PacketSize { get; } = Marshal.SizeOf<PartyFinderPacket>();

        internal int BatchNumber;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        private byte[] padding1;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        internal PartyFinderPacketListing[] Listings;
    }
    [StructLayout(LayoutKind.Sequential)]
    internal struct PartyFinderPacketListing
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        private byte[] header1;
        internal uint Id;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        private byte[] header2;

        internal uint ContentIdLower;
        private ushort unknownShort1;
        private ushort unknownShort2;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
        private byte[] header3;

        internal byte Category;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
        private byte[] header4;

        internal ushort Duty;
        internal byte DutyType;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 11)]
        private byte[] header5;

        internal ushort World;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        private byte[] header6;

        internal byte Objective;
        internal byte BeginnersWelcome;
        internal byte Conditions;
        internal byte DutyFinderSettings;
        internal byte LootRules;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
        private byte[] header7; // all zero in every pf I've examined

        internal uint LastPatchHotfixTimestamp; // last time the servers were restarted?
        internal ushort SecondsRemaining;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        private byte[] header8; // 00 00 01 00 00 00 in every pf I've examined

        internal ushort MinimumItemLevel;
        internal ushort HomeWorld;
        internal ushort CurrentWorld;

        private byte header9;

        internal byte NumSlots;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
        private byte[] header10;

        internal byte SearchArea;

        private byte header11;

        internal byte NumParties;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
        private byte[] header12; // 00 00 00 always. maybe numParties is a u32?

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        internal uint[] Slots;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        internal byte[] JobsPresent;

        // Note that ByValTStr will not work here because the strings are UTF-8 and there's only a CharSet for UTF-16 in C#.
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
        internal byte[] Name;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 192)]
        internal byte[] Description;
    }
}
