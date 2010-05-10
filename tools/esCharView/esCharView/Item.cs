﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace esCharView
{
	public class Item
	{
		public enum ItemQuality
		{
			Unknown, // Shouldn't happen
			Inferior,
			Normal,
			Superior,
			Magic,
			Set,
			Rare,
			Unique,
			Craft
		}

		public enum EquipmentLocation
		{
			None, // Not equipped
			Head,
			Amulet,
			Body,
			RightPrimary,
			LeftPrimary,
			RightRing,
			LeftRing,
			Belt,
			Feet,
			Gloves,
			RightSecondary,
			LeftSecondary
		}

		public enum ItemLocation
		{
			Stored,
			Equipped,
			Belt,
			Ground,
			Tohand,
			Dropping,
			Socketed
		}

		public enum StorageType
		{
			Unknown,
			Inventory,
			UnknownB,
			UnknownC,
			Cube,
			Stash,
		}

		private List<Item> sockets = new List<Item>();
		public List<Item> Sockets
		{
			get { return sockets; }
		}

		public string ItemCode { get; set; }
		public byte[] ItemData { get; set; }

		public bool IsEquipped { get; set; }
		public bool IsInSocket { get; set; }
		public bool IsIdentified { get; set; }
		public bool IsSwitchIn { get; set; }
		public bool IsSwitchOut { get; set; }
		public bool IsBroken { get; set; }
		public bool IsSocketed { get; set; }
		public bool IsStoreItem { get; set; }
		public bool IsEar { get; set; }
		public bool IsStarterItem { get; set; }
		public bool IsSimpleItem { get; set; }
		public bool IsEthereal { get; set; }
		public bool IsPersonalized { get; set; }
		public bool IsGamble { get; set; }
		public bool IsRuneword { get; set; }
		public ItemLocation Location { get; set; }
		public EquipmentLocation PositionOnBody { get; set; }
		public uint PositionX { get; set; }
		public uint PositionY { get; set; }
		public StorageType StorageId { get; set; }
		public uint GoldAmount { get; set; }
		public uint SocketsFilled { get; set; }
		public uint Id { get; set; }
		public uint Level { get; set; }
		public ItemQuality Quality { get; set; }
		public bool HasGraphic { get; set; }
		public uint Graphic { get; set; }
		public uint[] Prefix { get; set; }
		public uint[] Suffix { get; set; }
		public uint UniqueSetId { get; set; }
		public int Defense { get; set; }
		public uint MaxDurability { get; set; }
		public uint Durability { get; set; }
		public bool IsIndestructable { get; set; }
		public uint SocketCount { get; set; }
		public uint Quantity { get; set; }

		public BitArray ItemSetPropertyBits { get; set; }
		public uint[] ItemSetProperties { get; set; }
		public uint ItemProperties { get; set; }
		public uint RunewordProperties { get; set; }

		BitReader br;
		public long RemainingBits { get { return br.BitCount - br.Position; } }

		public Item(byte[] itemData)
		{
			ItemData = itemData;
			br = new BitReader(itemData);

			Prefix = new uint[3];
			Suffix = new uint[3];
			ItemSetPropertyBits = new BitArray(5);
			ItemSetProperties = new uint[5];

			ReadItemData();
		}

		/// <summary>
		/// Decodes raw item data
		/// </summary>
		private void ReadItemData()
		{
			ReadItemDataSimple();

			if (IsEar)
			{
				ReadItemDataEar();
				return;
			}

			ItemCode = br.ReadString(3, 8);
			br.SkipBits(8); // Skip null terminator of item code

			// Read gold data if it's gold
			// TODO: Should this come later on? Not sure how to test this...
			if (ItemCode.ToLower() == "gld")
			{
				ReadItemDataGold();
				return;
			}

			// Simple items don't have extended data
			if (!IsSimpleItem)
			{
				ReadItemDataExtended();
			}

			// TODO: Not sure what this bit is, I can't find any items with it set
			// Extend.txt says it's some sort of random flag followed by 40 bits
			if (br.ReadBoolean())
			{
				br.SkipBits(8);
				br.SkipBits(32);
			}

			// Type specific extended data
			if (!IsSimpleItem)
			{
				ReadItemDataExtendedSpecific();
			}
		}

		/// <summary>
		/// Decodes basic item data. All items have basic data
		/// </summary>
		private void ReadItemDataSimple()
		{
			br.SkipBits(16); // "JM" header

			IsEquipped = br.ReadBoolean();
			br.SkipBits(2); // Unknown 1
			IsInSocket = br.ReadBoolean();
			IsIdentified = br.ReadBoolean();
			br.SkipBits(1); // Unknown 2
			IsSwitchIn = br.ReadBoolean();
			IsSwitchOut = br.ReadBoolean();
			IsBroken = br.ReadBoolean();
			br.SkipBits(1); // Unknown 3 +
			br.SkipBits(1); // Potion (?)
			IsSocketed = br.ReadBoolean();
			br.SkipBits(1); // Unknown 4
			IsStoreItem = br.ReadBoolean();
			br.SkipBits(1); // NotInSocket (?)
			br.SkipBits(1); // Unknown 5
			IsEar = br.ReadBoolean();
			IsStarterItem = br.ReadBoolean();
			br.SkipBits(3); // Unknown 6
			IsSimpleItem = br.ReadBoolean();
			IsEthereal = br.ReadBoolean();
			br.SkipBits(1); // Any (?)
			IsPersonalized = br.ReadBoolean();
			IsGamble = br.ReadBoolean();
			IsRuneword = br.ReadBoolean();
			br.SkipBits(15);
			Location = (ItemLocation)br.Read(3);

			PositionOnBody = (EquipmentLocation)br.Read(4);
			PositionX = br.Read(4);
			PositionY = br.Read(4);
			StorageId = (StorageType)br.Read(3);
		}

		/// <summary>
		/// Decodes ear data
		/// </summary>
		private void ReadItemDataEar()
		{
			uint earClass = br.Read(3);
			uint earLevel = br.Read(7);
			// earName is the rest of the packet

			Level = earLevel;
			ItemCode = "ear";

			return;
		}

		/// <summary>
		/// Decodes gold data
		/// </summary>
		private void ReadItemDataGold()
		{
			if (br.ReadBoolean())
				GoldAmount = br.ReadUInt32();
			else
				GoldAmount = br.Read(12);

			return;
		}

		/// <summary>
		/// Decodes extended item data
		/// </summary>
		private void ReadItemDataExtended()
		{
			SocketsFilled = br.Read(3);

			Id = br.ReadUInt32();
			Level = br.Read(7);

			Quality = (ItemQuality)br.Read(4);

			HasGraphic = br.ReadBoolean();
			if (HasGraphic)
			{
				Graphic = br.Read(3);
			}

			// No idea what this flag is. Some say Class data others say Color data
			if (br.ReadBoolean())
			{
				br.SkipBits(11);
			}

			switch (Quality)
			{
				case ItemQuality.Inferior:
				case ItemQuality.Superior:
					br.SkipBits(3);
					break;
				case ItemQuality.Normal:
					if (ItemDefs.IsCharm(ItemCode))
					{
						br.SkipBits(12); // Not sure
					}
					else if (ItemDefs.IsScrollOrTome(ItemCode))
					{
						br.SkipBits(5); // Spell Id?
					}
					else if (ItemDefs.IsMonsterPart(ItemCode))
					{
						br.SkipBits(10); // Monster id?
					}
					break;
				case ItemQuality.Magic:
					Prefix[0] = br.Read(11);
					Suffix[0] = br.Read(11);
					break;

				case ItemQuality.Rare:
				case ItemQuality.Craft:
					br.SkipBits(8); // Prefix name id?
					br.SkipBits(8); // Suffix name id?

					for (int i = 0; i < 3; i++)
					{
						if (br.ReadBoolean())
						{
							Prefix[i] = br.Read(11);
						}
						if (br.ReadBoolean())
						{
							Suffix[i] = br.Read(11);
						}
					}
					break;

				case ItemQuality.Unique:
				case ItemQuality.Set:
					UniqueSetId = br.Read(12);
					break;

				default:
					break;
			}

			// TODO: check order of Runeword and personalized
			if (IsRuneword)
			{
				br.SkipBits(16);
			}
			// TODO: check order of Runeword and personalized
			if (IsPersonalized)
			{
				br.SkipBits(15);
			}
		}

		/// <summary>
		/// Decodes item specific data
		/// </summary>
		private void ReadItemDataExtendedSpecific()
		{
			if (ItemDefs.IsArmor(ItemCode))
			{
				// 1095 from ItemStatCost.txt. It just allows for a minimum of -1095 defense
				Defense = (int)br.Read(12) - 1095;
			}

			if (ItemDefs.IsWeapon(ItemCode) || ItemDefs.IsArmor(ItemCode))
			{
				MaxDurability = br.ReadByte();

				if (MaxDurability != 0)
				{
					Durability = br.ReadByte();
				}
				else
				{
					IsIndestructable = true;
				}
			}

			// Not sure of the order of socketed+stackable. Socketed arrows seem to mess up
			if (ItemDefs.IsStackable(ItemCode))
			{
				Quantity = br.Read(9);
			}
			if (IsSocketed)
			{
				SocketCount = br.Read(4);
			}

			if (Quality == ItemQuality.Set)
			{
				for (int i = 0; i < ItemSetPropertyBits.Length; i++)
				{
					ItemSetPropertyBits[i] = br.ReadBoolean();
				}
			}

			ReadItemDataProperty();

			if (Quality == ItemQuality.Set)
			{
				for (int i = 0; i < ItemSetPropertyBits.Length; i++)
				{
					if (ItemSetPropertyBits[i])
					{
						ReadItemDataProperty();
					}
				}
			}

			if (IsRuneword)
			{
				ReadItemDataProperty();
			}
		}

		// TODO: Not yet implemented, probably won't be since it's such a pain to do
		private void ReadItemDataProperty()
		{
			br.Read(9);
			//uint propertyId = 0;

			//while (br.Position < br.BitCount)
			//{
			//    propertyId = br.Read(9);
			//    if (propertyId == 0x1ff)
			//    {
			//        break;
			//    }

			//    //Read property specific bits
			//}
		}

		/// <summary>
		/// Attempts to remove a socketed item, this will fail horribly if used on runewords
		/// </summary>
		/// <param name="index">Index of socket to clear</param>
		public void RemoveSocketedItem(int index)
		{
			if (index < 0 || index > SocketsFilled)
			{
				return;
			}

			if (IsRuneword)
			{
				return;
			}

			Sockets.RemoveAt(index);
			SocketsFilled--;
		}

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();

			sb.Append(String.Format("{0} {1}", ItemCode, ItemDefs.GetItemDescription(ItemCode)));

			if (sockets.Count > 0)
			{
				sb.Append(" { ");
				for (int i = 0; i < sockets.Count; i++)
				{
					sb.Append(ItemDefs.GetItemDescription(sockets[i].ItemCode));

					if (i < sockets.Count - 1)
					{
						sb.Append(", ");
					}
				}
				sb.Append(" }");
			}

			return sb.ToString();
		}
	}
}
